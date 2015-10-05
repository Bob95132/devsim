/***
DEVSIM
Copyright 2013 Devsim LLC

This file is part of DEVSIM.

DEVSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, version 3.

DEVSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with DEVSIM.  If not, see <http://www.gnu.org/licenses/>.
***/

#ifndef EQUATIONCOMMANDS_HH
#define EQUATIONCOMMANDS_HH
class CommandHandler;
namespace dsCommand {
struct Commands;
extern Commands EquationCommands[];
void createEquationCmd(CommandHandler &);
void createInterfaceEquationCmd(CommandHandler &);
void createContactEquationCmd(CommandHandler &);
void createCustomEquationCmd(CommandHandler &);
void getEquationNumbersCmd(CommandHandler &);
void getEquationListCmd(CommandHandler &);
void getInterfaceEquationListCmd(CommandHandler &);
void getContactEquationListCmd(CommandHandler &);
void deleteEquationCmd(CommandHandler &);
void deleteInterfaceEquationCmd(CommandHandler &);
void deleteContactEquationCmd(CommandHandler &);
void deleteEquationCmd(CommandHandler &);
void deleteContactEquationCmd(CommandHandler &);
void deleteInterfaceEquationCmd(CommandHandler &);
}

#endif
