/*
  optionsdialog.cpp - Options Dialog implementation file of WildFox-Mimic.
  
  Revision 0
  
  Features:
        - 
        
  Notes:
        - 
        
  2021/02/02, Maya Posch
  (c) Nyanko.ws
*/


#include "optionsdialog.h"
#include "cookiemanagedialog.h"


// --- CONSTRUCTOR ---
OptionsDialog::OptionsDialog(CookieJar* cookiejar) {
    dialog.setupUi(this);
    
    cookies = cookiejar;
    
    // connections
    connect(dialog.optOkButton, SIGNAL(pressed()), this, SLOT(saveSettings()));
    connect(dialog.optCancelButton, SIGNAL(pressed()), this, SLOT(cancel()));
    connect(dialog.optButtonManageCookies, SIGNAL(pressed()), this, SLOT(manageCookies()));
    
    // load the current settings into the dialogue.
    // General tab
    
    // Privacy tab
    dialog.optCheckEnableCookies->setChecked(settings.value("cookiesEnabled", true).toBool());
    dialog.optCheckDisable3rdCookies->setChecked(settings.value("cookies3rdDisabled", true).toBool());
}


// --- DECONSTRUCTOR ---
OptionsDialog::~OptionsDialog() {
    settings.sync();
}


// --- MANAGE COOKIES ---
// Launches the manage cookies dialogue.
void OptionsDialog::manageCookies() {
    CookieManageDialog cmd(cookies->getDatabase(), cookies->getBuffer());
    cmd.exec();
}


// --- SAVE SETTINGS ---
// Saves the settings and closes the dialogue.
void OptionsDialog::saveSettings() {
    settings.setValue("cookiesEnabled", QVariant(dialog.optCheckEnableCookies->isChecked()));
    settings.setValue("cookies3rdEnabled", QVariant(!dialog.optCheckDisable3rdCookies->isChecked()));
    close();
}


// --- CANCEL ---
// Called when the dialogue is cancelled. No settings are saved. 
// Dialogue is closed.
void OptionsDialog::cancel() {
    close();
}
