#ifndef VOPA_MO_H
#define VOPA_MO_H

/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * vopamo.h
 *
 * Copyright (C) 2016 - Yann Collette
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <lv2.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"
#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

#define MIDI_COMMANDMASK 0xF0
#define MIDI_CHANNELMASK 0x0F

#define MIDI_CONTROL 0xB0

enum PORTS_VOPA_ST {
	PORT_VOPA_OUTPUT=0,
	PORT_VOPA_INPUT,
	PORT_VOPA_MIDI
};

void runVOPA_MO(LV2_Handle arg, uint32_t nframes);
LV2_Handle instantiateVOPA_MO(const LV2_Descriptor *descriptor,double s_rate, const char *path,const LV2_Feature * const* features);
void cleanupVOPA_MO(LV2_Handle instance);
void connectPortVOPA_MO(LV2_Handle instance, uint32_t port, void *data_location);

typedef struct VOPA_MO_t {
	float* output;
	const float* input;
	const LV2_Atom_Sequence* MidiIn;
	// Features
	LV2_URID_Map* map;
	LV2_URID midi_MidiEvent;
	unsigned int volume;
	unsigned int panning;
} VOPA_MO;

#endif
