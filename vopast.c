/*
 * vopast.c
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

#include "helper.h"

#include "vopast.h"

#define VOPA_PAN_DIV 127.0
#define VOPA_VOL_DIV 100.0

void runVOPA_ST(LV2_Handle arg, uint32_t nframes) {
  VOPA_ST* so = (VOPA_ST*)arg;
  float* left_outbuffer  = so->left_output;
  float* right_outbuffer = so->right_output;
  const float* left_inbuffer   = so->left_input;
  const float* right_inbuffer  = so->right_input;

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

  float tmp   = so->panning / VOPA_PAN_DIV;
  float pan_r = sqrt(1 - tmp);
  float pan_l = sqrt(tmp);
  float vol   = so->volume / VOPA_VOL_DIV;
  
  for(int i = 0; i < nframes; i++) {
    left_outbuffer[i]  = left_inbuffer[i]  * pan_r * vol;
    right_outbuffer[i] = right_inbuffer[i] * pan_l * vol;
  }
}

LV2_Handle instantiateVOPA_ST(const LV2_Descriptor *descriptor,double s_rate, const char *path,const LV2_Feature * const* features) {
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
  
  VOPA_ST* self = (VOPA_ST*)calloc(1, sizeof(VOPA_ST));
  self->map = map;
  self->midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
  self->volume  = 100;
  self->panning = 64;
  
  return self;
}

void cleanupVOPA_ST(LV2_Handle instance) {
  free(instance);
}

void connectPortVOPA_ST(LV2_Handle instance, uint32_t port, void *data_location) {
  VOPA_ST* so = (VOPA_ST*) instance;

  switch(port) {
  case PORT_VOPA_LEFT_OUTPUT:
    so->left_output = data_location;
    break;
  case PORT_VOPA_RIGHT_OUTPUT:
    so->right_output = data_location;
    break;
  case PORT_VOPA_LEFT_INPUT:
    so->left_input = data_location;
    break;
  case PORT_VOPA_RIGHT_INPUT:
    so->right_input = data_location;
    break;
  case PORT_VOPA_MIDI:
    so->MidiIn = (const LV2_Atom_Sequence*)data_location;
    break;
  default:
    fputs("Warning, unconnected port!\n",stderr);
  }
}

static const LV2_Descriptor descriptorVOPA_ST = {
  .URI            = "https://github.com/ycollet/vopa:VoPaSt",
  .instantiate    = instantiateVOPA_ST,
  .connect_port   = connectPortVOPA_ST,
  .activate       = NULL,
  .run            = runVOPA_ST,
  .deactivate     = NULL,
  .cleanup        = cleanupVOPA_ST,
  .extension_data = NULL,
};

LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  switch (index) {
  case 0:
    return &descriptorVOPA_ST;
  default:
    return NULL;
  }
}
