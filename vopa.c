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
  lv2_event_begin(&so->in_iterator, so->MidiIn);
  
  float* left_outbuffer = so->left_output;
  float* right_outbuffer = so->right_output;
  float* left_inbuffer = so->left_input;
  float* right_inbuffer = so->right_input;
  
  for(int i = 0; i < nframes; i++) {
    while(lv2_event_is_valid(&so->in_iterator)) {
      uint8_t* data;
      LV2_Event* event = lv2_event_get(&so->in_iterator, &data);
      if (event->type == 0) {
	so->event_ref->lv2_event_unref(so->event_ref->callback_data, event);
      } else if(event->type==so->midi_event_id) {
	if (event->frames > i) {
	  break;
	} else {
	  const uint8_t* evt = (uint8_t*)data;
	  if ((evt[0] & MIDI_COMMANDMASK)==MIDI_CONTROL) {
	    unsigned int command_val = evt[2];
	    switch(evt[1]) {
	    case 7:
	      so->volume = command_val;
	      break;
	    case 10:
	      so->panning = command_val;
	      break;
	    }
	  }
	}
      }
      
      lv2_event_increment(&so->in_iterator);
    }

    float pan_r = sqrt(1 - pow(((so->panning + 127.0) / 256.0), 2.0));
    float pan_l = (so->panning + 127.0) / 256.0;
    float vol   = so->volume / 127.0;
    left_outbuffer[i]  = (left_inbuffer[i] * pan_r + right_inbuffer[i] * pan_l) * vol;
    right_outbuffer[i] = (left_inbuffer[i] * pan_l + right_inbuffer[i] * pan_r) * vol;
  }
}

LV2_Handle instantiateVOPA(const LV2_Descriptor *descriptor,double s_rate, const char *path,const LV2_Feature * const* features) {
  VOPA* so = malloc(sizeof(VOPA));
  LV2_URI_Map_Feature * map_feature;
  const LV2_Feature * const * ft;
  
  for (ft = features; *ft; ft++) {
    if (!strcmp((*ft)->URI, "http://lv2plug.in/ns/ext/uri-map")) {
      map_feature = (*ft)->data;
      so->midi_event_id = map_feature->uri_to_id(map_feature->callback_data,
						 "http://lv2plug.in/ns/ext/event",
						 "http://lv2plug.in/ns/ext/midi#MidiEvent");
    } else if (!strcmp((*ft)->URI, "http://lv2plug.in/ns/ext/event")) {
      so->event_ref = (*ft)->data;
    }
  }

  puts( "VOPA v.1.0 by ycollet 2016" );
		
  so->volume  = 100;
  so->panning = 0;
  
  return so;
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
    so->MidiIn = data_location;
    break;
  default:
    fputs("Warning, unconnected port!\n",stderr);
  }
}
