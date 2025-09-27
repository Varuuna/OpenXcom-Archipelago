/*
 * Copyright 2010-2016 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ArchipelagoConnectState.h"
#include "../Engine/Game.h"
#include "../Engine/LocalizedText.h"
#include "../Mod/Mod.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Engine/Options.h"
#include "../Archipelago/ArchipelagoManager.h"
#include "NewGameState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Archipelago Connect screen.
 */
ArchipelagoConnectState::ArchipelagoConnectState() : _connecting(false)
{
    // Create objects
    _window = new Window(this, 320, 200, 0, 0);
    _btnConnect = new TextButton(100, 16, 60, 160);
    _btnCancel = new TextButton(100, 16, 160, 160);
    _txtTitle = new Text(300, 17, 10, 20);
    
    _txtServerUrl = new Text(120, 9, 10, 50);
    _edtServerUrl = new TextEdit(this, 200, 16, 110, 48);
    
    _txtSlotName = new Text(120, 9, 10, 75);
    _edtSlotName = new TextEdit(this, 200, 16, 110, 73);
    
    _txtPassword = new Text(120, 9, 10, 100);
    _edtPassword = new TextEdit(this, 200, 16, 110, 98);
    
    _txtStatus = new Text(300, 32, 10, 120);

    // Set palette
    setInterface("mainMenu");

    add(_window, "window", "mainMenu");
    add(_btnConnect, "button", "mainMenu");
    add(_btnCancel, "button", "mainMenu");
    add(_txtTitle, "text", "mainMenu");
    add(_txtServerUrl, "text", "mainMenu");
    add(_edtServerUrl, "text", "mainMenu");
    add(_txtSlotName, "text", "mainMenu");
    add(_edtSlotName, "text", "mainMenu");
    add(_txtPassword, "text", "mainMenu");
    add(_edtPassword, "text", "mainMenu");
    add(_txtStatus, "text", "mainMenu");

    centerAllSurfaces();

    // Set up objects
    _window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

    _txtTitle->setBig();
    _txtTitle->setAlign(ALIGN_CENTER);
    _txtTitle->setText(tr("STR_ARCHIPELAGO_CONNECT"));

    _txtServerUrl->setText(tr("STR_ARCHIPELAGO_SERVER_URL"));
    _edtServerUrl->setBig();
    _edtServerUrl->onChange((ActionHandler)&ArchipelagoConnectState::edtServerUrlChange);

    _txtSlotName->setText(tr("STR_ARCHIPELAGO_SLOT_NAME"));
    _edtSlotName->setBig();
    _edtSlotName->onChange((ActionHandler)&ArchipelagoConnectState::edtSlotNameChange);

    _txtPassword->setText(tr("STR_ARCHIPELAGO_PASSWORD"));
    _edtPassword->setBig();
    _edtPassword->onChange((ActionHandler)&ArchipelagoConnectState::edtPasswordChange);

    _btnConnect->setText(tr("STR_ARCHIPELAGO_CONNECT"));
    _btnConnect->onMouseClick((ActionHandler)&ArchipelagoConnectState::btnConnectClick);
    _btnConnect->onKeyboardPress((ActionHandler)&ArchipelagoConnectState::btnConnectClick, Options::keyOk);

    _btnCancel->setText(tr("STR_CANCEL"));
    _btnCancel->onMouseClick((ActionHandler)&ArchipelagoConnectState::btnCancelClick);
    _btnCancel->onKeyboardPress((ActionHandler)&ArchipelagoConnectState::btnCancelClick, Options::keyCancel);

    _txtStatus->setAlign(ALIGN_CENTER);
    _txtStatus->setWordWrap(true);
    _txtStatus->setText("");

    // Set default values
    _edtServerUrl->setText("localhost:38281");
    _edtSlotName->setText("Player2");
}

/**
 * Cleans up the Archipelago Connect state.
 */
ArchipelagoConnectState::~ArchipelagoConnectState()
{
}

/**
 * Handler for clicking the Connect button.
 * @param action Pointer to an action.
 */
void ArchipelagoConnectState::btnConnectClick(Action *)
{
    if (_connecting)
        return;

    if (!validateInput())
        return;

    startConnection();
}

/**
 * Handler for clicking the Cancel button.
 * @param action Pointer to an action.
 */
void ArchipelagoConnectState::btnCancelClick(Action *)
{
    _game->popState();
}

/**
 * Handler for changing the server URL.
 * @param action Pointer to an action.
 */
void ArchipelagoConnectState::edtServerUrlChange(Action *)
{
    _connectionInfo.serverUrl = _edtServerUrl->getText();
}

/**
 * Handler for changing the slot name.
 * @param action Pointer to an action.
 */
void ArchipelagoConnectState::edtSlotNameChange(Action *)
{
    _connectionInfo.slotName = _edtSlotName->getText();
}

/**
 * Handler for changing the password.
 * @param action Pointer to an action.
 */
void ArchipelagoConnectState::edtPasswordChange(Action *)
{
    _connectionInfo.password = _edtPassword->getText();
}

/**
 * Update the connection status.
 */
void ArchipelagoConnectState::think()
{
    State::think();
    
    if (_connecting)
    {
        ArchipelagoManager* manager = ArchipelagoManager::getInstance();
        APConnectionStatus status = manager->getConnectionStatus();
        
        switch (status)
        {
            case APConnectionStatus::Connected:
                updateStatus(tr("STR_ARCHIPELAGO_CONNECTING"));
                break;
            case APConnectionStatus::Authenticated:
                onConnectionSuccess();
                break;
            case APConnectionStatus::ConnectionRefused:
                onConnectionFailure(tr("STR_ARCHIPELAGO_CONNECTION_FAILED"));
                break;
            case APConnectionStatus::Disconnected:
                if (_connecting)
                {
                    onConnectionFailure(tr("STR_ARCHIPELAGO_CONNECTION_FAILED"));
                }
                break;
        }
    }
}

/**
 * Initialize the state.
 */
void ArchipelagoConnectState::init()
{
    State::init();
    _edtServerUrl->setFocus(true);
}

/**
 * Validate input fields.
 * @return true if input is valid
 */
bool ArchipelagoConnectState::validateInput()
{
    if (_edtServerUrl->getText().empty())
    {
        updateStatus(tr("STR_ARCHIPELAGO_ERROR_NO_SERVER"));
        return false;
    }
    
    if (_edtSlotName->getText().empty())
    {
        updateStatus(tr("STR_ARCHIPELAGO_ERROR_NO_SLOT"));
        return false;
    }
    
    return true;
}

/**
 * Update status text.
 * @param message Status message
 */
void ArchipelagoConnectState::updateStatus(const std::string& message)
{
    _txtStatus->setText(message);
}

/**
 * Handle successful connection.
 */
void ArchipelagoConnectState::onConnectionSuccess()
{
    _connecting = false;
    updateStatus(tr("STR_ARCHIPELAGO_CONNECTED"));
    
    // Connection successful, proceed to new game
    _game->popState();
    _game->pushState(new NewGameState);
}

/**
 * Handle connection failure.
 * @param error Error message
 */
void ArchipelagoConnectState::onConnectionFailure(const std::string& error)
{
    _connecting = false;
    updateStatus(error);
    
    // Re-enable connect button
    _btnConnect->setVisible(true);
}

/**
 * Start the connection process.
 */
void ArchipelagoConnectState::startConnection()
{
    _connecting = true;
    updateStatus(tr("STR_ARCHIPELAGO_CONNECTING"));
    
    // Disable connect button during connection
    _btnConnect->setVisible(false);
    
    // Update connection info from form
    _connectionInfo.serverUrl = _edtServerUrl->getText();
    _connectionInfo.slotName = _edtSlotName->getText();
    _connectionInfo.password = _edtPassword->getText();
    
    // Start connection through manager
    ArchipelagoManager* manager = ArchipelagoManager::getInstance();
    manager->initialize(_game);
    
    if (!manager->connect(_connectionInfo))
    {
        onConnectionFailure(tr("STR_ARCHIPELAGO_CONNECTION_FAILED"));
    }
}

}