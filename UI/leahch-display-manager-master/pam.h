#ifndef _PAM_H_
#define _PAM_H_

#include <stdbool.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

pam_handle_t *p_check_login(const char *username, const char *password, const char *service_name);
int p_login(pam_handle_t *pam_hanlde, char *resource);
bool p_logout(pam_handle_t *pam_handle);

#endif /* _PAM_H_ */
