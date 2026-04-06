#pragma once
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

#include "../Engine/State.h"
#include "../Archipelago/ArchipelagoTypes.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextEdit;

/**
 * Archipelago Connection window that allows the player
 * to connect to an Archipelago server before starting a new game.
 */
class ArchipelagoConnectState : public State
{
private:
    TextButton *_btnConnect, *_btnCancel;
    Window *_window;
    Text *_txtTitle, *_txtServerUrl, *_txtSlotName, *_txtPassword, *_txtStatus;
    TextEdit *_edtServerUrl, *_edtSlotName, *_edtPassword;
    
    APConnectionInfo _connectionInfo;
    bool _connecting;
    
public:
    /// Creates the Archipelago Connect state.
    ArchipelagoConnectState();
    /// Cleans up the Archipelago Connect state.
    ~ArchipelagoConnectState();
    /// Handler for clicking the Connect button.
    void btnConnectClick(Action *action);
    /// Handler for clicking the Cancel button.
    void btnCancelClick(Action *action);
    /// Handler for changing the server URL.
    void edtServerUrlChange(Action *action);
    /// Handler for changing the slot name.
    void edtSlotNameChange(Action *action);
    /// Handler for changing the password.
    void edtPasswordChange(Action *action);
    /// Update the connection status.
    void think();
    /// Initialize the state.
    void init();
    
private:
    /// Validate input fields.
    bool validateInput();
    /// Update status text.
    void updateStatus(const std::string& message);
    /// Handle successful connection.
    void onConnectionSuccess();
    /// Handle connection failure.
    void onConnectionFailure(const std::string& error);
    /// Start the connection process.
    void startConnection();
};

}