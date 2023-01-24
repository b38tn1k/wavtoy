#include "score_handler.h"
#include "frequencies.h"

vector<string> splitStringOnSpace(string str, char splitter = ' '){
    vector<string> res;
    string s;
    for (string::iterator i = str.begin(); i != str.end(); ++i) {
        if (*i == splitter) {
            res.push_back(s);
            s = "";
        } else {
            s += *i;
        }
    }
    res.push_back(s);
    return res;
}

ScoreHandler::ScoreHandler(string filename){
    // LOAD TXT
    fstream f;
    string contents;
    vector<string> lines;
    f.open(filename);
    if (f.is_open()) {
        while (getline(f, contents)){
            if (string(contents).find("#") == -1) {
                lines.push_back(contents);
            }
        };
    } else {
        cout << "Read Error" << endl;
        // return 0;
    }
    // EXAMINE TXT CONTENTS
    title = lines[0]; // title
    bpm = stof(lines[1]); //bpm

    int notationBegin = readHead(lines);
    makeNoteMap();
    
    double div = 0;
    length = 0;
    currentSeq = "START";
    sequences[currentSeq] = seq(currentSeq);

    for (int i = notationBegin; i < lines.size(); i++){
        div = readScoreLine(lines[i], div);
        cleanScoreForEditor(lines[i]);
    }
    sequences[currentSeq].length = length;
    buildScore();
}

void ScoreHandler::buildScore() {
    double sIndex = 0.0;
    vector < noteEvent > tempScore;
    for (vector < vector <string> > :: iterator j = structureStrings.begin(); j != structureStrings.end(); ++j){
        double maxSeqLength = 0.0;
        for (vector <string> ::iterator k = j->begin(); k != j->end(); ++k) {
            for (vector <noteEvent> :: iterator n = sequences[*k].score.begin(); n != sequences[*k].score.end(); ++n){
                tempScore.push_back(offsetNEIndex(*n, sIndex));
            }
            if (sequences[*k].length > maxSeqLength) {
                maxSeqLength = sequences[*k].length;
            }
        }
        sIndex += maxSeqLength;
    }
    score = tempScore;
    length = sIndex;
}

noteEvent ScoreHandler::offsetNEIndex(noteEvent nE, double off) {
    double frequency = nE.frequency;
    double beatIndex = nE.beatIndex + off;
    u_int beatDivision = nE.beatDivision;
    u_int instrument = nE.instrument;
    return noteEvent(frequency, beatIndex, beatDivision, instrument);
}

double ScoreHandler::readScoreLine(string line, double div){
    if (line.length() == 0) {
        return div;
    }
    vector <string> temp = splitStringOnSpace(line);
    if (temp[0].find("DIV") != -1) {
        div = stof(temp[1]);
    } else if (temp[0].find("REST") != -1) {
        length += (4.0/div);
    } else if (temp[0].find("SEC") != -1) {
        sequences[currentSeq].length = length;
        currentSeq = line;
        sequences[currentSeq] = seq(line);
        length = 0; //restart the thing for another SEC
    } else {
        int inst = 0;
        for (vector <string>::iterator k = begin(temp); k != end(temp); ++ k) {
            if (string(*k).find("|") != -1) {
                inst += 1;
            } else {
                if (notes.count(string(*k))>= 1) {
                    sequences[currentSeq].score.push_back(noteEvent(notes[string(*k)], length, div, inst));
                }
            }
        }
        length += (4.0/div);
    }
    return div;
}

void ScoreHandler::cleanScoreForEditor(string line){
    vector <string> cleaner = splitStringOnSpace(line, '|');
    for (vector <string>::iterator j = cleaner.begin(); j != cleaner.end(); ++j){
        string tj = *j;
        const size_t trimStart = tj.find_first_not_of(" \t");
        const size_t trimEnd = tj.find_last_not_of(" \t");
        if (trimStart != string::npos) {
            *j = tj.substr(trimStart, (trimEnd - trimStart + 1));
        } else {
            *j = " ";
        }
    }
    while (cleaner.size() < instrumentStrings.size()) {
        cleaner.push_back(" ");
    }
    scoreStrings.push_back(cleaner);
}

int ScoreHandler::readHead(vector<string> lines){
    int notationBegin = 0;
    int j = 0;
    bool structure = false;
    for(vector<string>::iterator it = begin(lines); it != end(lines); ++it){
        j++;
        if ((string(*it).find("SYNTH") != -1) || (string(*it).find("KICK") != -1) || (string(*it).find("NOISE") != -1)) {
            instrumentStrings.push_back(splitStringOnSpace(string(*it)));
            notationBegin = j;
        }
        if (string(*it).find("FX") != -1) {
            notationBegin = j;
            string fxIndex = to_string(instrumentStrings.size()-1);
            vector <string> fxString = splitStringOnSpace(string(*it));
            // hacky
            instrumentStrings[stoi(fxIndex)].insert(instrumentStrings[stoi(fxIndex)].end(), fxString.begin(),fxString.end());
            // hacky
            fxString[0] = fxIndex;
            fxStrings.push_back(fxString);
        }
        if (string(*it).find("STRUCTURE") != -1) {
            structure = true;
            notationBegin = j;
        }

        if (structure == true && (string(*it).find("SEC") != -1)) {
            structureStrings.push_back(splitStringOnSpace(string(*it)));
        }

        if (string(*it).find("END STRUCTURE") != -1) {
            structure = false;
            notationBegin = j;
        }
    }
    return notationBegin + 1;
}

void ScoreHandler::makeNoteMap() {
    notes["C0"] = C0;
    notes["C0S"] = C0S;
    notes["D0"] = D0;
    notes["D0S"] = D0S;
    notes["E0"] = E0;
    notes["F0"] = F0;
    notes["F0S"] = F0S;
    notes["G0"] = G0;
    notes["G0S"] = G0S;
    notes["A0"] = A0;
    notes["A0S"] = A0S;
    notes["C1"] = C1;
    notes["C1S"] = C1S;
    notes["D1"] = D1;
    notes["D1S"] = D1S;
    notes["E1"] = E1;
    notes["F1"] = F1;
    notes["F1S"] = F1S;
    notes["G1"] = G1;
    notes["G1S"] = G1S;
    notes["A1"] = A1;
    notes["A1S"] = A1S;
    notes["C2"] = C2;
    notes["C2S"] = C2S;
    notes["D2"] = D2;
    notes["D2S"] = D2S;
    notes["E2"] = E2;
    notes["F2"] = F2;
    notes["F2S"] = F2S;
    notes["G2"] = G2;
    notes["G2S"] = G2S;
    notes["A2"] = A2;
    notes["A2S"] = A2S;
    notes["C3"] = C3;
    notes["C3S"] = C3S;
    notes["D3"] = D3;
    notes["D3S"] = D3S;
    notes["E3"] = E3;
    notes["F3"] = F3;
    notes["F3S"] = F3S;
    notes["G3"] = G3;
    notes["G3S"] = G3S;
    notes["A3"] = A3;
    notes["A3S"] = A3S;
    notes["C4"] = C4;
    notes["C4S"] = C4S;
    notes["D4"] = D4;
    notes["D4S"] = D4S;
    notes["E4"] = E4;
    notes["F4"] = F4;
    notes["F4S"] = F4S;
    notes["G4"] = G4;
    notes["G4S"] = G4S;
    notes["A4"] = A4;
    notes["A4S"] = A4S;
    notes["C5"] = C5;
    notes["C5S"] = C5S;
    notes["D5"] = D5;
    notes["D5S"] = D5S;
    notes["E5"] = E5;
    notes["F5"] = F5;
    notes["F5S"] = F5S;
    notes["G5"] = G5;
    notes["G5S"] = G5S;
    notes["A5"] = A5;
    notes["A5S"] = A5S;
    notes["C6"] = C6;
    notes["C6S"] = C6S;
    notes["D6"] = D6;
    notes["D6S"] = D6S;
    notes["E6"] = E6;
    notes["F6"] = F6;
    notes["F6S"] = F6S;
    notes["G6"] = G6;
    notes["G6S"] = G6S;
    notes["A6"] = A6;
    notes["A6S"] = A6S;
    notes["C7"] = C7;
    notes["C7S"] = C7S;
    notes["D7"] = D7;
    notes["D7S"] = D7S;
    notes["E7"] = E7;
    notes["F7"] = F7;
    notes["F7S"] = F7S;
    notes["G7"] = G7;
    notes["G7S"] = G7S;
    notes["A7"] = A7;
    notes["A7S"] = A7S;
    notes["C8"] = C8;
    notes["C8S"] = C8S;
    notes["D8"] = D8;
    notes["D8S"] = D8S;
    notes["E8"] = E8;
    notes["F8"] = F8;
    notes["F8S"] = F8S;
    notes["G8"] = G8;
    notes["G8S"] = G8S;
    notes["A8"] = A8;
    notes["A8S"] = A8S;
}