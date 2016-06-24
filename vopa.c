/*
 * vopa.c
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

#include "vopa.h"

void runVOPA(LV2_Handle arg, uint32_t nframes) {
  VOPA* so = (VOPA*)arg;
  float* left_outbuffer = so->left_output;
  float* right_outbuffer = so->right_output;
  float* left_inbuffer = so->left_input;
  float* right_inbuffer = so->right_input;

  LV2_ATOM_SEQUENCE_FOREACH(so->MidiIn, ev) {
    if (ev->body.type == so->uris.midi_MidiEvent) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      
      if ((lv2_midi_message_type(msg) & MIDI_COMMANDMASK)==MIDI_CONTROL) {
	unsigned int command_val = msg[2];
	
	switch(msg[1]) {
	case 7:
	  so->volume = command_val;
	  break;
	case 10:
	  so->panning = command_val;
	  break;
	}
      }
    }
    
    for(int i = 0; i < nframes; i++) {
      float pan_r = sqrt(1 - pow(((so->panning + 127.0) / 256.0), 2.0));
      float pan_l = (so->panning + 127.0) / 256.0;
      float vol   = so->volume / 127.0;
      left_outbuffer[i]  = (left_inbuffer[i] * pan_r + right_inbuffer[i] * pan_l) * vol;
      right_outbuffer[i] = (left_inbuffer[i] * pan_l + right_inbuffer[i] * pan_r) * vol;
    }
  }
}

LV2_Handle instantiateVOPA(const LV2_Descriptor *descriptor,double s_rate, const char *path,const LV2_Feature * const* features) {
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
  
  VOPA* self = (VOPA*)calloc(1, sizeof(VOPA));
  self->map = map;
  self->uris.midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
  
  puts( "VOPA v.1.0 by ycollet 2016" );
		
  self->volume  = 100;
  self->panning = 0;
  
  return self;
}

void cleanupVOPA(LV2_Handle instance) {
  free(instance);
}

void connectPortVOPA(LV2_Handle instance, uint32_t port, void *data_location) {
  VOPA* so = (VOPA*) instance;
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
