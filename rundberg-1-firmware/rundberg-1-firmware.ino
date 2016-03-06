// GLOVE CONSTANTS

const int palm = A0; 
const int indexFinger = A2; 
const int middleFinger = A4; 
const int ringFinger = A6; 
const int thumb = A8;

const int leftHand[] = {palm, thumb, indexFinger, middleFinger, ringFinger};
const int THRESHOLDS[] = {5, 3, 40, 30, 30};

// THRESHOLDS

const int THRESHOLD = 30;
const int smoothLength = 10;
const int MIDI_NOTE_START = 36;

// VALUES

int leftHandSensorValues[] = {0, 0, 0, 0, 0};

boolean isPressed[] = {false, false, false, false, false};
boolean didChange[] = {true, true, true, true, true};

// Create Hand Array

void setup()
{
  Serial.begin(9600); // USB is always 12 Mbit/sec
  pinMode(14, INPUT); 
  pinMode(16, INPUT); 
  pinMode(18, INPUT); 
  pinMode(20, INPUT); 
  pinMode(22, INPUT); 
  
}

String createCsv(int* values) {
    String ret = "";
    for (int i = 0; i < 4; i++) {
        ret += String(values[i]) + ",";
    }
    ret += String(values[4]);
    return ret;
}

String createBoolCsv(boolean* values) {
    String ret = "";
    for (int i = 0; i < 4; i++) {
        ret += String(values[i]) + ",";
    }
    ret += String(values[4]);
    return ret;
}

void sendMidiSignals(boolean* values) {
    int velocity = 100;
    int channel = 0;
    for (int i = 0; i < 5; i++) {
        int note = MIDI_NOTE_START+i;
        if (isPressed[i] && didChange[i]) {
            usbMIDI.sendNoteOn(note, velocity, channel);
        } else if (didChange[i]) {
            usbMIDI.sendNoteOff(note, 0, channel);
        }
    }
}

void fillIsPressed(int* values) {
    for (int i = 0; i < 5; i++) {
        boolean v = values[i] > THRESHOLDS[i];
        didChange[i] = v != isPressed[i];
        isPressed[i] = v;
    }
}

// the loop routine runs over and over again forever:
void loop()
{ 
 
  for (int i = 0; i < 5; i++) {
    int raw = analogRead(leftHand[i]);
    int old = leftHandSensorValues[i];
    
    // didn't really think this through
    // it is smoothing stuff
    leftHandSensorValues[i] = 
        (old * smoothLength + raw) / (smoothLength + 1);
  }

  String valCsv = createCsv(leftHandSensorValues);
  fillIsPressed(leftHandSensorValues);
  String boolCsv = createBoolCsv(isPressed);
  
  sendMidiSignals(isPressed);
  
  Serial.println(valCsv);
  
  delay(5);

}
