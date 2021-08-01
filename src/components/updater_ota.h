// SPDX-License-Identifier: GPL-3.0-or-later
/*
    Copyright (C) 2021  The Home Control Authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef UPDATER_OTA_H
#define UPDATER_OTA_H

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <functional>
#include "ticker_component.h"

class updater_ota: public ticker_component
{
private:
    bool use_serial, run_default_handlers;
    void setup();
    void handle_on_start();
    void handle_on_end();
    void handle_on_error(ota_error_t error);
    void handle_on_progress(unsigned int current, unsigned int total);

public:
    typedef std::function<void()> callback;
    typedef std::function<void(unsigned int)> callback_progress;
    typedef std::function<void(ota_error_t)> callback_error;

    updater_ota();
    void set_hostname(const String& name, bool append_chip_id=true);
    void set_password(const String& password);
    void begin(bool use_serial=true, bool run_default_handers=true);
    void update();

    callback on_start;
    callback on_end;
    callback_error on_error;
    callback_progress on_progress;
};

#endif // UPDATER_OTA_H
