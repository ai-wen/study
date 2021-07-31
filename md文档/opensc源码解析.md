```cpp



//-------------------------src\libopensc\card.c
int _sc_match_atr(sc_card_t *card, const struct sc_atr_table *table, int *type_out)
{
	int res;

	if (card == NULL)
		return -1;
	res = match_atr_table(card->ctx, table, &card->atr);
	if (res < 0)
		return res;
	if (type_out != NULL)
		*type_out = table[res].type;
	return res;
}

static int match_atr_table(sc_context_t *ctx, const struct sc_atr_table *table, struct sc_atr *atr)
{
	u8 *card_atr_bin;
	size_t card_atr_bin_len;
	char card_atr_hex[3 * SC_MAX_ATR_SIZE];
	size_t card_atr_hex_len;
	unsigned int i = 0;

	if (ctx == NULL || table == NULL || atr == NULL)
		return -1;
	card_atr_bin = atr->value;
	card_atr_bin_len = atr->len;
	sc_bin_to_hex(card_atr_bin, card_atr_bin_len, card_atr_hex, sizeof(card_atr_hex), ':');
	card_atr_hex_len = strlen(card_atr_hex);

	sc_debug(ctx, SC_LOG_DEBUG_MATCH, "ATR     : %s", card_atr_hex);

	for (i = 0; table[i].atr != NULL; i++) {
		const char *tatr = table[i].atr;
		const char *matr = table[i].atrmask;
		size_t tatr_len = strlen(tatr);
		u8 mbin[SC_MAX_ATR_SIZE], tbin[SC_MAX_ATR_SIZE];
		size_t mbin_len, tbin_len, s, matr_len;
		size_t fix_hex_len = card_atr_hex_len;
		size_t fix_bin_len = card_atr_bin_len;

		sc_debug(ctx, SC_LOG_DEBUG_MATCH, "ATR try : %s", tatr);

		if (tatr_len != fix_hex_len) {
			sc_debug(ctx, SC_LOG_DEBUG_MATCH, "ignored - wrong length");
			continue;
		}
		if (matr != NULL) {
			sc_debug(ctx, SC_LOG_DEBUG_MATCH, "ATR mask: %s", matr);

			matr_len = strlen(matr);
			if (tatr_len != matr_len)
				continue;
			tbin_len = sizeof(tbin);
			sc_hex_to_bin(tatr, tbin, &tbin_len);
			mbin_len = sizeof(mbin);
			sc_hex_to_bin(matr, mbin, &mbin_len);
			if (mbin_len != fix_bin_len) {
				sc_debug(ctx, SC_LOG_DEBUG_MATCH, "length of atr and atr mask do not match - ignored: %s - %s", tatr, matr);
				continue;
			}
			for (s = 0; s < tbin_len; s++) {
				/* reduce tatr with mask */
				tbin[s] = (tbin[s] & mbin[s]);
				/* create copy of card_atr_bin masked) */
				mbin[s] = (card_atr_bin[s] & mbin[s]);
			}
			if (memcmp(tbin, mbin, tbin_len) != 0)
				continue;
		} else {
			if (strncasecmp(tatr, card_atr_hex, tatr_len) != 0)
				continue;
		}
		return i;
	}
	return -1;
}


//-------------------------.\src\libopensc\opensc.h
struct sc_reader_driver {
	const char *name;
	const char *short_name;
	struct sc_reader_operations *ops;

	void *dll; //加载相应的库
};

//-------------------------.\src\libopensc\ctx.c:
struct _sc_driver_entry {
	const char *name;
	void *(*func)(void);
};
static const struct _sc_driver_entry internal_card_drivers[] = {
#ifdef ENABLE_OPENSSL
	{ "entersafe",(void *(*)(void)) sc_get_entersafe_driver },
#ifdef ENABLE_SM
	{ "epass2003",(void *(*)(void)) sc_get_epass2003_driver },
#endif
    ...
}


#define SC_MAX_CARD_DRIVERS		48

int sc_establish_context(sc_context_t **ctx_out, const char *app_name)
{
	sc_context_param_t ctx_param;

	memset(&ctx_param, 0, sizeof(sc_context_param_t));
	ctx_param.ver      = 0;
	ctx_param.app_name = app_name;
	return sc_context_create(ctx_out, &ctx_param);
}

int sc_context_create(sc_context_t **ctx_out, const sc_context_param_t *parm)
{
    set_defaults(ctx, &opts)
    {
        add_internal_drvs(opts);
        //static void add_internal_drvs(struct _sc_ctx_options *opts)
        {
                const struct _sc_driver_entry *lst;
                int i;

                lst = internal_card_drivers;
                i = 0;
                while (lst[i].name != NULL) 
                {
                    add_drv(opts, lst[i].name);  
                    //static void add_drv(struct _sc_ctx_options *opts, const char *name)
                    {
                        struct _sc_driver_entry *lst;
                        int *cp, max, i;

                        lst = opts->cdrv;
                        cp = &opts->ccount;
                        max = SC_MAX_CARD_DRIVERS;
                        if (*cp == max) /* No space for more drivers... */
                            return;
                        for (i = 0; i < *cp; i++)
                            if (strcmp(name, lst[i].name) == 0)
                                return;
                        lst[*cp].name = strdup(name);

                        *cp = *cp + 1;
                    }
                    i++;
                }
        }
    }
    ...
    //创建相应的reader_driver ,加载对应的dll  sc_reader_driver
#ifdef ENABLE_PCSC
	ctx->reader_driver = sc_get_pcsc_driver();
#elif defined(ENABLE_CRYPTOTOKENKIT)
	ctx->reader_driver = sc_get_cryptotokenkit_driver();
#elif defined(ENABLE_CTAPI)
	ctx->reader_driver = sc_get_ctapi_driver();
#elif defined(ENABLE_OPENCT)
	ctx->reader_driver = sc_get_openct_driver();
#endif
    ...
    load_card_drivers(ctx, &opts);
    //static int load_card_drivers(sc_context_t *ctx, struct _sc_ctx_options *opts)
    {
        ...
        for (j = 0; internal_card_drivers[j].name != NULL; j++) {
                    if (strcmp(ent->name, internal_card_drivers[j].name) == 0) {
                        func = (struct sc_card_driver *(*)(void)) internal_card_drivers[j].func;
                        break;
                    }
                }
        
        *(void **)(tfunc) = load_dynamic_driver(ctx, &dll, ent->name);
        ...
    }

	load_card_atrs(ctx);
    //static int load_card_atrs(sc_context_t *ctx)
    {
        ...
        _sc_add_atr(ctx, driver, &t);
        ...
    }
    ...
}




//-------------------------src\libopensc\cards.h
/* EnterSafe cards */
SC_CARD_TYPE_ENTERSAFE_3K=19000+1,
SC_CARD_TYPE_ENTERSAFE_FTCOS_EPASS2003=19000+3,

extern sc_card_driver_t *sc_get_epass2003_driver(void);
extern sc_card_driver_t *sc_get_entersafe_driver(void);

//-------------------------src\libopensc\card-entersafe.c
static const struct sc_atr_table entersafe_atrs[] = {
	{ 
		 "3b:0f:00:65:46:53:05:19:05:71:df:00:00:00:00:00:00", 
		 "ff:ff:ff:ff:ff:ff:ff:00:ff:ff:ff:00:00:00:00:00:00", 
		 "ePass3000", SC_CARD_TYPE_ENTERSAFE_3K, 0, NULL },
         ...
}

static struct sc_card_operations entersafe_ops; //opensc.h
struct sc_card_driver * sc_get_entersafe_driver(void)
{
	return sc_get_driver();
    //static struct sc_card_driver * sc_get_driver(void)
    {
        entersafe_ops.match_card = entersafe_match_card;
    }
}

static int entersafe_match_card(sc_card_t *card)
{
	int i;
	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	i = _sc_match_atr(card, entersafe_atrs, &card->type);
	if (i < 0)
		return 0;		

	return 1;
}


//-------------------------src\libopensc\card-epass2003.c
static const struct sc_atr_table epass2003_atrs[] = {
	/* This is a FIPS certified card using SCP01 security messaging. */
	{"3B:9F:95:81:31:FE:9F:00:66:46:53:05:10:00:11:71:df:00:00:00:6a:82:5e",
	 "FF:FF:FF:FF:FF:00:FF:FF:FF:FF:FF:FF:00:00:00:ff:00:ff:ff:00:00:00:00",
	 "FTCOS/ePass2003", SC_CARD_TYPE_ENTERSAFE_FTCOS_EPASS2003, 0, NULL },
	{NULL, NULL, NULL, 0, 0, NULL}
};        

struct sc_card_driver *sc_get_epass2003_driver(void)
{
	return sc_get_driver();
}




//-------------------------src\libopensc\opensc.h
typedef struct sc_card_driver {
	const char *name;
	const char *short_name;             //epass2003
	struct sc_card_operations *ops;
	struct sc_atr_table *atr_map;
	unsigned int natrs;
	void *dll;
} sc_card_driver_t;


//-------------------------\src\pkcs15init\pkcs15-lib.c
static struct profile_operations {
	const char *name;
	void *func;
} profile_operations[] = {
	{ "epass2003",(void*) sc_pkcs15init_get_epass2003_ops },
	{ NULL, NULL },
};

//.\src\minidriver\minidriver.c:  rv = sc_pkcs15init_bind(card, "pkcs15", NULL, NULL, &profile);
//.\src\pkcs11\framework-pkcs15.c:                rc = sc_pkcs15init_bind(p11card->card, "pkcs15", NULL, NULL, &profile);
//#ifdef USE_PKCS15_INIT
//#include "pkcs15init/pkcs15-init.h"
//#endif

int sc_pkcs15init_bind(struct sc_card *card, const char *name, const char *profile_option,
		struct sc_app_info *app_info, struct sc_profile **result)
{
    ...
    profile_operations
    ...
    if (!func) {
		/* no builtin support for this driver => look if there's a
		 * dynamic module for this card */
		func = (struct sc_pkcs15init_operations *(*)(void)) load_dynamic_driver(card->ctx, &profile->dll, driver);
        //static void * load_dynamic_driver(struct sc_context *ctx, void **dll,const char *name)
        {
            libname = find_library(ctx, name);
            {
                //static const char *find_library(struct sc_context *ctx, const char *name)
                {
                    int          i;
                    const char   *libname = NULL;
                    scconf_block *blk, **blocks;

                    for (i = 0; ctx->conf_blocks[i]; i++) {
                        blocks = scconf_find_blocks(ctx->conf, ctx->conf_blocks[i], "framework", "pkcs15");
                        if (!blocks)
                            continue;
                        blk = blocks[0];
                        free(blocks);
                        if (blk == NULL)
                            continue;
                        blocks = scconf_find_blocks(ctx->conf, blk, "pkcs15init", name);
                        if (!blocks)
                            continue;
                        blk = blocks[0];
                        free(blocks);
                        if (blk == NULL)
                            continue;
                        libname = scconf_get_str(blk, "module", NULL);
                        break;
                    }
                    if (!libname) {
                        sc_log(ctx, "unable to locate pkcs15init driver for '%s'", name);
                    }
                    return libname;
                }
            }
        }
	}
}

//-------------------------\src\pkcs15init\pkcs15-init.h
extern struct sc_pkcs15init_operations *sc_pkcs15init_get_epass2003_ops(void);

//-------------------------\src\pkcs15init\pkcs15-epass2003.c
struct sc_pkcs15init_operations *sc_pkcs15init_get_epass2003_ops(void)
{
	return &sc_pkcs15init_epass2003_operations;
    static struct sc_pkcs15init_operations sc_pkcs15init_epass2003_operations = {
	epass2003_pkcs15_erase_card,
	epass2003_pkcs15_init_card,
	epass2003_pkcs15_create_dir,
	NULL,			/* create_domain */
	epass2003_pkcs15_pin_reference,
	epass2003_pkcs15_create_pin,
	epass2003_pkcs15_key_reference,
	epass2003_pkcs15_create_key,
	epass2003_pkcs15_store_key,
	epass2003_pkcs15_generate_key,
	NULL, NULL,		/* encode private/public key */
	NULL,			/* finalize */
	epass2003_pkcs15_delete_object,
	NULL, NULL, NULL, NULL, NULL,	/* pkcs15init emulation */
	epass2003_pkcs15_sanity_check,
    };
}





//-------------------------win32/customactions.cpp
typedef struct _MD_REGISTRATION
{
	TCHAR szName[256];
	BYTE pbAtr[256];
	DWORD dwAtrSize;    
	BYTE pbAtrMask[256];
} MD_REGISTRATION, *PMD_REGISTRATION;
MD_REGISTRATION minidriver_registration[] = {
	{TEXT("ePass2003"),{0x3b,0x9f,0x95,0x81,0x31,0xfe,0x9f,0x00,0x66,0x46,0x53,0x05,0x01,0x00,0x11,0x71,0xdf,0x00,0x00,0x03,0x6a,0x82,0xf8},
                   23, {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}},


WcaGlobalInitialize(hInst);
hr = WcaInitialize(hInstall, "AddSmartCardConfiguration");
WcaGlobalFinalize();
hr = WcaInitialize(hInstall, "RemoveSmartCardConfiguration");

UINT WINAPI AddSmartCardConfiguration(MSIHANDLE hInstall)
{
	HRESULT hr = S_OK;
	UINT er = ERROR_SUCCESS;
	int i ;

	hr = WcaInitialize(hInstall, "AddSmartCardConfiguration");
	ExitOnFailure(hr, "Failed to initialize");

	WcaLog(LOGMSG_STANDARD, "Initialized.");

	for (i = 0; i < sizeof(minidriver_registration) / sizeof(MD_REGISTRATION); i++)
	{
		RegisterSmartCard(minidriver_registration + i);
	}

	/* never fails or only if the msi install functions didn't work. If the install custom action trigger a failure, the user is unable to install the software */
LExit:
	er = SUCCEEDED(hr) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;
	return WcaFinalize(er);
}

```

```cpp
-E Attr

一、编译
https://github.com/OpenSC/OpenSC/wiki/Compiling-on-Windows
在 win32/Make.rules.mak中
配置:
WIX = D:\SOFT\WiXToolsetv3.11
放开openssl注释：
OPENSSL_DEF= /DENABLE_OPENSSL

nmake -f Makefile.mak
nmake -f Makefile.mak clean

1、默认使用pcsc
#ifdef ENABLE_PCSC
.\config.h:#define ENABLE_PCSC 1
.\win32\config.h:#define ENABLE_PCSC
.\win32\winconfig.h:#define ENABLE_PCSC

在在 src\libopensc\cards.c中添加
/* EnterSafe cards */
	SC_CARD_TYPE_ENTERSAFE_BASE = 19000,
	SC_CARD_TYPE_ENTERSAFE_CRYPTOIDE,

在 src\libopensc\card-entersafe.c中添加
static const struct sc_atr_table entersafe_atrs[] = {
	{ 
		 "3b:9f:11:81:31:fe:9f:00:6a:6d:54:6f:6b:65:6e:2d:46:00:00:81:90:00:00", 
		 "ff:ff:ff:ff:ff:ff:ff:ff:ff:ff:ff:ff:ff:ff:ff:ff:ff:00:00:ff:ff:ff:00", 
		 "CryptoIDE", SC_CARD_TYPE_ENTERSAFE_CRYPTOIDE, 0, NULL },
         ...
}

windows：在 win32/customactions.cpp中添加， 可选？
typedef struct _MD_REGISTRATION
{
	TCHAR szName[256];
	BYTE pbAtr[256];
	DWORD dwAtrSize;    
	BYTE pbAtrMask[256];
} MD_REGISTRATION, *PMD_REGISTRATION;
MD_REGISTRATION minidriver_registration[] ={
    ...
    {TEXT("CryptoIDE"),						  {0x3b,0x9f,0x11,0x81,0x31,0xfe,0x9f,0x00,0x6a,0x6d,0x54,0x6f,0x6b,0x65,0x6e,0x2d,0x46,0x00,0x00,0x81,0x90,0x00,0x00},
										  23, {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0xff,0x00}},
    ...
}




2、私有智能卡指令
在 win32/Make.rules.mak中
放开注释 SM_DEF = /DENABLE_SM
fips Secure Messaging
.\config.h:#define ENABLE_SM 1





二、使用
windows
配置文件
/*
//Computer\HKEY_CURRENT_USER\Software\OpenSC Project\OpenSC\ConfigFile
//G:\\OpenSC-0.21.0\\opensc.conf
app default {
	# debug = 3;
	# debug_file = opensc - debug.txt;
	framework pkcs15{
		# use_file_caching = true;
	}
	reader_driver pcsc {
		# Limit command and response sizes. Some Readers don't propagate their
		# transceive capabilities correctly. max_send_size and max_recv_size
		# allow setting the limits manually, for example to enable extended
		# length capabilities.
		# Default: max_send_size = 255, max_recv_size = 256;
		# max_send_size = 65535;
		# max_recv_size = 65536;
		#
		# Connect to reader in exclusive mode?
		# Default: false
		# connect_exclusive = true;
		#
		# What to do when disconnecting from a card (SCardDisconnect)
		# Valid values: leave, reset, unpower.
		# Default: leave
		# disconnect_action = reset;
		#
		# What to do at the end of a transaction (SCardEndTransaction)
		# Valid values: leave, reset, unpower.
		# Default: leave
		# transaction_end_action = reset;
		#
		# What to do when reconnection to a card (SCardReconnect)
		# Valid values: leave, reset, unpower.
		# Note that this affects only the internal reconnect (after a SCARD_W_RESET_CARD).
		# A forced reset via sc_reset() always does a full powerup.
		# Default: leave
		# reconnect_action = reset;
		#
		# Enable pinpad if detected (PC/SC v2.0.2 Part 10)
		# Default: true
		# enable_pinpad = false;
		#
		# Some pinpad readers can only handle one exact length of the PIN.
		# fixed_pinlength sets this value so that OpenSC expands the padding to
		# this length.
		# Default: 0 (i.e. not fixed)
		# fixed_pinlength = 6;
		#
		# Detect reader capabilities with escape commands (wrapped APDUs with
		# CLA=0xFF as defined by PC/SC pt. 3 and BSI TR-03119, e.g. for getting
		# the UID, escaped PIN commands and the reader's firmware version)
		# Default: false
		# enable_escape = true;
		#
		# Use specific pcsc provider.
		# Default: winscard.dll
		# provider_library = winscard.dll
	}
}
*/




int main()
{
	const char *opt_driver = NULL;
	const char *opt_conf_entry = NULL;
	const char *opt_reset_type = NULL;

	int err = 0, long_optind = 0;

	struct sc_reader *reader = NULL;
	sc_context_param_t ctx_param;

	memset(&ctx_param, 0, sizeof(ctx_param));
	ctx_param.ver = 0;
	//ctx_param.app_name = app_name;

	err = sc_context_create(&ctx, &ctx_param);
	if (SC_SUCCESS != err)
		return err;

	
	unsigned int i;
	/* Automatically try to skip to a reader with a card if reader not specified */
	for (i = 0; i < sc_ctx_get_reader_count(ctx); i++) 
	{
		reader = sc_ctx_get_reader(ctx, i);

		if (sc_detect_card_presence(reader) & SC_READER_CARD_PRESENT)
		{
			fprintf(stderr, "Using reader with a card: %s\n", (reader)->name); //Longmai mToken CryptoIDE 0
			break;
		}
	}
	
	//util_hex_dump_asc(stdout, reader->atr.value, reader->atr.len, -1);
	char tmp[SC_MAX_ATR_SIZE * 3];
	sc_bin_to_hex(reader->atr.value, reader->atr.len, tmp, sizeof(tmp) - 1, ':'); //3b:9f:11:81:31:fe:9f:00:6a:6d:54:6f:6b:65:6e:2d:46:00:00:81:90:00:79
	fprintf(stdout, "%s\n", tmp);


	opt_driver = "epass2003";//
	err = sc_set_card_driver(ctx, opt_driver);

	err = sc_connect_card(reader, &card);
	if (SC_SUCCESS != err)
		return err;

	sc_serial_number_t serial;
	err = sc_lock(card);
	if (err == SC_SUCCESS)
		err = sc_card_ctl(card, SC_CARDCTL_GET_SERIALNR, &serial);
	sc_unlock(card);
	
	

	sc_disconnect_card(card);
	sc_release_context(ctx);
}

```