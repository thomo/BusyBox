/* 
 * based on 
 * https://github.com/mathertel/RotaryEncoder
 * 
 * See http://www.mathertel.de/License.aspx
 * Software License Agreement (BSD License)
 * Copyright (c) 2005-2014 by Matthias Hertel,  http://www.mathertel.de/
 * 
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer. 
 * - Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution. 
 * - Neither the name of the copyright owners nor the names of its contributors
 *   may be used to endorse or promote products derived from this software 
 *   without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "rotary.h"

#define LATCH0 0 // input state at position 0
#define LATCH3 3 // input state at position 3

// The array holds the values 
// - -1 for the entries where a position was decremented,
// - 1 for the entries where the position was incremented, and
// - 0 in all the other (no change or not valid) cases.

const signed char KNOBDIR[] = {
    0, -1, 1, 0,
    1, 0, 0, -1,
    -1, 0, 0, 1,
    0, 1, -1, 0
};

volatile unsigned char _rotaryOldState;

// positions: [3] 1 0 2 [3] 1 0 2 [3]
// [3] is the positions where my rotary switch detends
// ==> right, count up
// <== left,  count down
volatile unsigned char _rotaryPosition;  // Internal position (4 times rotaryEncoderPos)

// call this function every some milliseconds or by using an interrupt 
// for handling state changes of the rotary encoder.
void rotaryEncoderTick(unsigned char state) {

    if (_rotaryOldState != state) {
        signed char delta = KNOBDIR[state | (_rotaryOldState << 2)];
        
        switch (delta) {
            case -1:    
                if (_rotaryPosition != 0) {
                    _rotaryPosition -= 1;
                } 
                break;
            case 1:     
                if (_rotaryPosition != 255) {
                    _rotaryPosition += 1; 
                }
                break;
        }

        _rotaryOldState = state;

        switch (rotaryMode) {
            case ROTARY_MODE_FOUR3:
                if (state == LATCH3) {
                    // The hardware has 4 steps with a latch on the input state 3
                    rotaryEncoderPos = (unsigned char) _rotaryPosition >> 2;
                }
                break;

            case ROTARY_MODE_FOUR0:
                if (state == LATCH0) {
                    // The hardware has 4 steps with a latch on the input state 0
                    rotaryEncoderPos = (unsigned char) _rotaryPosition >> 2;
                }
                break;

            case ROTARY_MODE_TWO03:
                if ((state == LATCH0) || (state == LATCH3)) {
                    // The hardware has 2 steps with a latch on the input state 0 and 3
                    rotaryEncoderPos = (unsigned char) _rotaryPosition >> 1;
                }
                break;
        } 
    }
}