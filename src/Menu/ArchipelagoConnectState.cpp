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
#include "../Engine/Logger.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Archipelago/ArchipelagoManager.h"
#include "../Mod/Mod.h"
#include "NewGameState.h"
#include "../Engine/Options.h"
#include "../Archipelago/ArchipelagoTypes.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Archipelago Connect window.
 */
ArchipelagoConnectState::ArchipelagoConnectState() : _connecting(false), _lastConnectionState(AP_DISCONNECTED), _connectionTimeout(0)
{
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(300, 17, 10, 20);
	
	_txtServerUrl = new Text(100, 9, 10, 50);
	_edtServerUrl = new TextEdit(this, 200, 16, 110, 48);
	
	_txtServerPort = new Text(100, 9, 10, 75);
	_edtServerPort = new TextEdit(this, 200, 16, 110, 73);
	
	_txtSlotName = new Text(100, 9, 10, 100);
	_edtSlotName = new TextEdit(this, 200, 16, 110, 98);
	
	_txtPassword = new Text(100, 9, 10, 125);
	_edtPassword = new TextEdit(this, 200, 16, 110, 123);
	
	_txtStatus = new Text(300, 17, 10, 150);
	
	_btnConnect = new TextButton(100, 16, 60, 170);
	_btnCancel = new TextButton(100, 16, 170, 170);

	// Set palette
	setInterface("mainMenu");

	add(_window, "window", "mainMenu");
	add(_txtTitle, "text", "mainMenu");
	add(_txtServerUrl, "text", "mainMenu");
	add(_edtServerUrl, "text", "mainMenu");
	add(_txtServerPort, "text", "mainMenu");
	add(_edtServerPort, "text", "mainMenu");
	add(_txtSlotName, "text", "mainMenu");
	add(_edtSlotName, "text", "mainMenu");
	add(_txtPassword, "text", "mainMenu");
	add(_edtPassword, "text", "mainMenu");
	add(_txtStatus, "text", "mainMenu");
	add(_btnConnect, "button", "mainMenu");
	add(_btnCancel, "button", "mainMenu");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK01.SCR"));

	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_ARCHIPELAGO_CONNECT"));

	_txtServerUrl->setText(tr("STR_ARCHIPELAGO_SERVER_URL"));
	_edtServerUrl->setBig();
	_edtServerUrl->setText("localhost");

	_txtServerPort->setText(tr("STR_ARCHIPELAGO_SERVER_PORT"));
	_edtServerPort->setBig();
	_edtServerPort->setText("38281");

	_txtSlotName->setText(tr("STR_ARCHIPELAGO_SLOT_NAME"));
	_edtSlotName->setBig();
	_edtSlotName->setText("Player2");

	_txtPassword->setText(tr("STR_ARCHIPELAGO_PASSWORD"));
	_edtPassword->setBig();

	_txtStatus->setAlign(ALIGN_CENTER);
	_txtStatus->setText("");

	_btnConnect->setText(tr("STR_ARCHIPELAGO_CONNECT"));
	_btnConnect->onMouseClick((ActionHandler)&ArchipelagoConnectState::btnConnectClick);

	_btnCancel->setText(tr("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&ArchipelagoConnectState::btnCancelClick);
}

/**
 *
 */
ArchipelagoConnectState::~ArchipelagoConnectState()
{

}

/**
 * Attempts to connect to the Archipelago server.
 * @param action Pointer to an action.
 */
void ArchipelagoConnectState::btnConnectClick(Action *)
{
	std::string serverUrl = _edtServerUrl->getText();
	std::string serverPortStr = _edtServerPort->getText();
	std::string slotName = _edtSlotName->getText();
	std::string password = _edtPassword->getText();

	// Validate input
	if (serverUrl.empty())
	{
		updateStatus(tr("STR_ARCHIPELAGO_ERROR_NO_SERVER"));
		return;
	}
	
	if (slotName.empty())
	{
		updateStatus(tr("STR_ARCHIPELAGO_ERROR_NO_SLOT"));
		return;
	}

	int serverPort = 38281; // Default Archipelago port
	try
	{
		if (!serverPortStr.empty())
		{
			serverPort = std::stoi(serverPortStr);
		}
	}
	catch (...)
	{
		updateStatus(tr("STR_ARCHIPELAGO_ERROR_INVALID_PORT"));
		return;
	}

	updateStatus(tr("STR_ARCHIPELAGO_CONNECTING"));

	// Create Archipelago configuration
	ArchipelagoConfig config;
	config.hostname = serverUrl;
	config.port = serverPort;
	config.slot_name = slotName;
	config.password = password;

	// Attempt connection
	ArchipelagoManager* manager = ArchipelagoManager::getInstance();
	manager->setEnabled(true);
	manager->setConfig(config);
	
	// Start the connection process (this is asynchronous)
	Log(LOG_INFO) << "ArchipelagoConnectState: Attempting to connect to " << serverUrl << ":" << serverPort;
	if (manager->connect())
	{
		updateStatus(tr("STR_ARCHIPELAGO_CONNECTING"));
		// Disable the connect button to prevent multiple attempts
		_btnConnect->setVisible(false);
		_connecting = true;
		// Get the actual initial state from the manager instead of assuming CONNECTING
		_lastConnectionState = manager->getConnectionState();
		_connectionTimeout = CONNECTION_TIMEOUT_FRAMES;
		Log(LOG_INFO) << "ArchipelagoConnectState: Connection initiated, timeout set to " << CONNECTION_TIMEOUT_FRAMES << " frames";
	}
	else
	{
		Log(LOG_ERROR) << "ArchipelagoConnectState: Failed to initiate connection";
		updateStatus(tr("STR_ARCHIPELAGO_CONNECTION_FAILED"));
	}
}

/**
	* Runs state functionality every cycle.
	* Monitors the Archipelago connection status.
	*/
void ArchipelagoConnectState::think()
{
	State::think();
	
	if (_connecting)
	{
		ArchipelagoManager* manager = ArchipelagoManager::getInstance();
		ArchipelagoConnectionState currentState = manager->getConnectionState();
		
		// Decrement timeout counter
		_connectionTimeout--;
		
		// Log state changes for debugging
		if (currentState != _lastConnectionState)
		{
			Log(LOG_INFO) << "ArchipelagoConnectState: Connection state changed from "
			              << static_cast<int>(_lastConnectionState) << " to "
			              << static_cast<int>(currentState);
		}
		
		// Always update status to show current state for debugging
		std::string statusText;
		switch (currentState)
		{
			case AP_DISCONNECTED:
				statusText = "State: DISCONNECTED";
				break;
			case AP_CONNECTING:
				statusText = "State: CONNECTING... (" + std::to_string(_connectionTimeout / 60) + "s remaining)";
				break;
			case AP_CONNECTED:
				statusText = "State: CONNECTED (authenticating...) (" + std::to_string(_connectionTimeout / 60) + "s remaining)";
				break;
			case AP_AUTHENTICATED:
				statusText = "State: AUTHENTICATED - Success!";
				break;
			case AP_ERROR:
				statusText = "State: ERROR - Connection failed";
				break;
			default:
				statusText = "State: UNKNOWN (" + std::to_string(static_cast<int>(currentState)) + ")";
				break;
		}
		
		// Add connection info if available
		if (manager->isAuthenticated())
		{
			statusText += " | " + manager->getPlayerInfoString();
		}
		
		updateStatus(statusText);
		
		// Log timeout warnings
		if (_connectionTimeout <= 60 && _connectionTimeout % 60 == 0) // Every second in the last minute
		{
			Log(LOG_WARNING) << "ArchipelagoConnectState: Connection timeout warning - "
			                 << (_connectionTimeout / 60) << " seconds remaining, current state: "
			                 << static_cast<int>(currentState);
		}
		
		// Check for timeout
		if (_connectionTimeout <= 0 && currentState != AP_AUTHENTICATED)
		{
			Log(LOG_ERROR) << "ArchipelagoConnectState: Connection timeout reached, current state: "
			               << static_cast<int>(currentState);
			
			// Provide more specific error messages based on state
			std::string errorMsg;
			switch (currentState)
			{
				case AP_CONNECTING:
					errorMsg = "Connection timeout - unable to reach server";
					break;
				case AP_CONNECTED:
					errorMsg = "Authentication timeout - server may have rejected credentials";
					break;
				default:
					errorMsg = "Connection timeout - check server and credentials";
					break;
			}
			
			updateStatus(errorMsg);
			manager->disconnect();
			_connecting = false;
			_btnConnect->setVisible(true);
			return;
		}
		
		if (currentState != _lastConnectionState)
		{
			_lastConnectionState = currentState;
			
			switch (currentState)
			{
				case AP_AUTHENTICATED:
					Log(LOG_INFO) << "ArchipelagoConnectState: Successfully authenticated, proceeding to game";
					_connecting = false;
					// Connection successful, proceed to difficulty selection
					_game->popState();
					_game->pushState(new NewGameState);
					break;
				case AP_ERROR:
					Log(LOG_ERROR) << "ArchipelagoConnectState: Connection error occurred";
					_connecting = false;
					_btnConnect->setVisible(true);
					break;
				case AP_DISCONNECTED:
					Log(LOG_WARNING) << "ArchipelagoConnectState: Connection disconnected";
					_connecting = false;
					_btnConnect->setVisible(true);
					break;
			}
		}
	}
}

/**
 * Returns to the main menu.
 * @param action Pointer to an action.
 */
void ArchipelagoConnectState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Updates the connection status text.
 * @param status Status message to display.
 */
void ArchipelagoConnectState::updateStatus(const std::string &status)
{
	_txtStatus->setText(status);
}

}
