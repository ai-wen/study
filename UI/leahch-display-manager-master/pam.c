#include <security/pam_appl.h>
#include <security/pam_misc.h>

#include <pwd.h>
#include <grp.h>
#include <paths.h>

#include "pam.h"

// #define SERVICE_NAME "tcdm"
/*
#define err(name)                                   \
    do {                                            \
        fprintf(stderr, "%s: %s\n", name,           \
                pam_strerror(pam_handle, result));  \
        end(result);                                \
        return false;                               \
    } while (1);                                    \
*/

static void init_env(struct passwd *pw, pam_handle_t *pamh);
static void set_env(char *name, char *value, pam_handle_t *pamh);
// static int end(int last_result);

static int conv(int num_msg, const struct pam_message **msg,
                struct pam_response **resp, void *appdata_ptr);


pam_handle_t *p_check_login(const char *username, const char *password, const char* service_name) {
    pam_handle_t *pam_handle = NULL;
    const char *data[2] = {username, password};
    struct pam_conv pam_conv = {
        conv, data
    };

    int result = pam_start(service_name, NULL, &pam_conv, &pam_handle);

    if (result == PAM_SUCCESS)
        result = pam_set_item(pam_handle, PAM_USER, username);

    if (result == PAM_SUCCESS)
    	result = pam_authenticate(pam_handle, 0);

    if (result == PAM_SUCCESS)
    	result = pam_acct_mgmt(pam_handle, 0);

    if (result == PAM_SUCCESS)
    	result = pam_setcred(pam_handle, PAM_ESTABLISH_CRED);

    if (result == PAM_SUCCESS)
    	result = pam_open_session(pam_handle, 0);

    // SUCCESS RETURN
    if (result == PAM_SUCCESS)
	return pam_handle;

    // UNSUCCESS RETURN
    if (pam_handle != NULL)
	    pam_end(pam_handle, result);

    return NULL;
}


int p_login(pam_handle_t *pam_handle, char *resource) {
    int pid = fork();

    if (pid == 0) {
	char *username;
        pam_get_item(pam_handle, PAM_USER, (const void **)&username);
        struct passwd *pw = getpwnam(username);
        init_env(pw, pam_handle);

        chdir(pw->pw_dir);
        // We don't use ~/.xinitrc because we should already be in the users home directory
        //char *cmd = "exec /bin/bash --login .xinitrc";
        execle(pw->pw_shell, pw->pw_shell, "-c", resource, NULL, pam_getenvlist(pam_handle));
        //execl(pw->pw_shell, pw->pw_shell, "-c", cmd, NULL);
        printf("Failed to start window manager");
        exit(1);
	return -1;
    }

    return pid;
}



bool p_logout(pam_handle_t *pam_handle) {

    if (pam_handle == NULL)
	return false;

    int result = pam_close_session(pam_handle, 0);
    if (result != PAM_SUCCESS) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        fprintf(stderr, "pam close session: %s\n", pam_strerror(pam_handle, result));
    }

    result = pam_setcred(pam_handle, PAM_DELETE_CRED);
    if (result != PAM_SUCCESS) {
        fprintf(stderr, "pam delete cred: %s\n", pam_strerror(pam_handle, result));
    }

    if(pam_end(pam_handle, result) != PAM_SUCCESS) {
        fprintf(stderr, "pam end: %s\n", pam_strerror(pam_handle, result));
    }
    return true;
}

static void init_env(struct passwd *pw, pam_handle_t *pamh) {
    char * display = getenv("DISPLAY");
    printf("DISPLAY=%s\n", display);
    set_env("DISPLAY", display, pamh);

    char *xauthority = getenv("XAUTHORITY");
    printf("XAUTHORITY=%s\n", xauthority);
    if(strlen(xauthority) > 0) {
    	set_env("XAUTHORITY", xauthority, pamh);
    }

    set_env("HOME", pw->pw_dir, pamh);
    set_env("PWD", pw->pw_dir, pamh);
    set_env("SHELL", pw->pw_shell, pamh);
    set_env("USER", pw->pw_name, pamh);
    set_env("LOGNAME", pw->pw_name, pamh);
    set_env("PATH", "/usr/local/sbin:/usr/local/bin:/usr/bin:/bin", pamh);
    set_env("MAIL", _PATH_MAILDIR, pamh);

    if(setgid(pw->pw_gid) != 0) {
    	printf("Set setgid() ERROR!");
	exit(1);
    }
    
    if(setegid(pw->pw_gid) != 0) {
    	printf("Set setgid() ERROR!");
	exit(1);
    }
   
    if(initgroups(pw->pw_name, pw->pw_gid) != 0) {
    	printf("Set initgroups() ERROR!");
	exit(1);
    }
    if(setuid(pw->pw_uid) != 0) {
    	printf("Set setuid() ERROR!");
	exit(1);
    }

    /*
    char *xauthority = malloc(strlen(pw->pw_dir) + strlen("/.Xauthority") + 3);
    strcpy(xauthority, pw->pw_dir);
    strcat(xauthority, "/.Xauthority");
    strcat(xauthority, display);
    set_env("XAUTHORITY", xauthority);
    free(xauthority);
    */
}

static void set_env(char *name, char *value, pam_handle_t *pamh) {
    char *name_value = malloc(strlen(name) + strlen(value) + 2);
    strcpy(name_value, name);
    strcat(name_value, "=");
    strcat(name_value, value);
    pam_putenv(pamh, name_value); // TODO: Handle errors
    free(name_value);
}

/*
static int end(int last_result) {
    int result = pam_end(pam_handle, last_result);
    pam_handle = 0;
    return result;
}
*/

static int conv(int num_msg, const struct pam_message **msg,
                 struct pam_response **resp, void *appdata_ptr) {
    int i;

    *resp = calloc(num_msg, sizeof(struct pam_response));
    if (*resp == NULL) {
        return PAM_BUF_ERR;
    }

    int result = PAM_SUCCESS;
    for (i = 0; i < num_msg; i++) {
        char *username, *password;
        switch (msg[i]->msg_style) {
        case PAM_PROMPT_ECHO_ON:
            username = ((char **) appdata_ptr)[0];
            (*resp)[i].resp = strdup(username);
            break;
        case PAM_PROMPT_ECHO_OFF:
            password = ((char **) appdata_ptr)[1];
            (*resp)[i].resp = strdup(password);
            break;
        case PAM_ERROR_MSG:
            fprintf(stderr, "%s\n", msg[i]->msg);
            result = PAM_CONV_ERR;
            break;
        case PAM_TEXT_INFO:
            printf("%s\n", msg[i]->msg);
            break;
        }
        if (result != PAM_SUCCESS) {
            break;
        }
    }

    if (result != PAM_SUCCESS) {
        free(*resp);
        *resp = 0;
    }

    return result;
}

