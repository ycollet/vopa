/*
 * vopamo.c
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

#include <math.h>

#include "vopamo.h"

void runVOPA_MO(LV2_Handle arg, uint32_t nframes) {
  VOPA_MO* so = (VOPA_MO*)arg;
  float* outbuffer = so->output;
  const float* inbuffer = so->input;

  LV2_ATOM_SEQUENCE_FOREACH(so->MidiIn, ev) {
    if (ev->body.type == so->midi_MidiEvent) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      
      if ((lv2_midi_message_type(msg) & MIDI_COMMANDMASK)==MIDI_CONTROL) {
	switch(msg[1]) {
	case 7:
	  so->volume = msg[2];
	  break;
	case 10:
	  so->panning = msg[2];
	  break;
	}
      }
    }
  }

  float vol = so->volume / 127.0;
  
  for(int i = 0; i < nframes; i++) {
    outbuffer[i]  = inbuffer[i] * vol;
  }
}

LV2_Handle instantiateVOPA_MO(const LV2_Descriptor *descriptor,double s_rate, const char *path,const LV2_Feature * const* features) {
  LV2_URID_Map* map = NULL;
  for (int i = 0; features[i]; ++i) {
    if (!strcmp(features[i]->URI, LV2_URID__map)) {
      map = (LV2_URID_Map*)features[i]->data;
      break;
    }
  }
  
  if (!map) {
    return NULL;
  }
  
  VOPA_MO* self = (VOPA_MO*)calloc(1, sizeof(VOPA_MO));
  self->map = map;
  self->midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
  self->volume  = 100;
  
  return self;
}

void cleanupVOPA_MO(LV2_Handle instance) {
  free(instance);
}

void connectPortVOPA_MO(LV2_Handle instance, uint32_t port, void *data_location) {
  VOPA_MO* so = (VOPA_MO*) instance;

  switch(port) {
  case PORT_VOPA_OUTPUT:
    so->output = data_location;
    break;
  case PORT_VOPA_INPUT:
    so->input = data_location;
    break;
  case PORT_VOPA_MIDI:
    so->MidiIn = (const LV2_Atom_Sequence*)data_location;
    break;
  default:
    fputs("Warning, unconnected port!\n",stderr);
  }
}

static const LV2_Descriptor descriptorVOPA_MO = {
  .URI            = "https://github.com/ycollet/vopa:VoPaMo",
  .instantiate    = instantiateVOPA_MO,
  .connect_port   = connectPortVOPA_MO,
  .activate       = NULL,
  .run            = runVOPA_MO,
  .deactivate     = NULL,
  .cleanup        = cleanupVOPA_MO,
  .extension_data = NULL,
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &descriptorVOPA_MO;
  default:
    return NULL;
  }
}
