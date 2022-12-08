/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2015 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#pragma once

#include "InputDriver.h"

class JoystickInputDriver : public InputDriver
{
public:
  JoystickInputDriver() = default;
  void run() override;
  bool open() override;
  void close() override;
  [[nodiscard]] const std::string& get_name() const override;
  [[nodiscard]] std::string get_info() const override;
  void setJoystickNr(std::string jnr);

  [[nodiscard]] size_t getButtonCount() const override {
    return buttons;
  }
  [[nodiscard]] size_t getAxisCount() const override {
    return axes;
  }

private:
  int fd{-1};
  int version{0};
  std::string nr{"0"};
  unsigned char axes{0};
  unsigned char buttons{0};
  char name[1024];
  volatile bool stopRequest{false};
};
