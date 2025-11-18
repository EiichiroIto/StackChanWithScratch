#ifndef __SETTINGS__
#define __SETTINGS__

#define APP_NAME "stackchan"
#define SETTING_LENGTH 20

typedef struct {
  const char *key;
  char *var;
  const char *name;
  bool required;
} Settings;

extern void init_settings();
extern int NumSettings;
extern Settings AppSettings[];

#endif
