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
 * Archipelago Connection window for connecting to
 * an Archipelago multiworld server.
 */
class ArchipelagoConnectState : public State
{
private:
	TextButton *_btnConnect, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtServerUrl, *_txtServerPort, *_txtSlotName, *_txtPassword;
	TextEdit *_edtServerUrl, *_edtServerPort, *_edtSlotName, *_edtPassword;
	Text *_txtStatus;
	
	bool _connecting;
	ArchipelagoConnectionState _lastConnectionState;
	int _connectionTimeout;
	static const int CONNECTION_TIMEOUT_FRAMES = 600; // 10 seconds at 60 FPS (reasonable timeout)

public:
	/// Creates the Archipelago Connect state.
	ArchipelagoConnectState();
	/// Cleans up the Archipelago Connect state.
	~ArchipelagoConnectState();
	/// Handler for clicking the Connect button.
	void btnConnectClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Updates the connection status.
	void updateStatus(const std::string &status);
	/// Runs state functionality every cycle.
	void think() override;
};

}