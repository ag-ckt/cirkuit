/***************************************************************************
*   Copyright (C) 2011 by Matteo Agostinelli                              *
*   agostinelli@gmail.com                                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
***************************************************************************/
#ifndef CIRKUIT_MACROS_H
#define CIRKUIT_MACROS_H

#include <kdemacros.h>
#include <KPluginFactory>
#include <KPluginLoader>

#define CIRKUIT_VERSION 1
 
/**
  Exports Backend plugin.
*/
#define K_EXPORT_CIRKUIT_PLUGIN(libname, classname) \
    K_PLUGIN_FACTORY(factory, registerPlugin<classname>();) \
    K_EXPORT_PLUGIN(factory("cirkuit_" #libname)) \
    K_EXPORT_PLUGIN_VERSION(CIRKUIT_VERSION)

#endif /* CIRKUIT_MACROS_H */
