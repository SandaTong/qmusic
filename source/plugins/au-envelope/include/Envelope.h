/*
                          qmusic

    Copyright (C) 2015 Arthur Benilov,
    arthur.benilov@gmail.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.
*/

#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include "AudioUnit.h"

class QtVariantProperty;

class Envelope : public AudioUnit
{
public:

    // Envelope state
    enum State {
        State_Off,
        State_Attack,
        State_Decay,
        State_Sustain,
        State_Release
    };


    Envelope(AudioUnitPlugin *pPlugin);
    ~Envelope();

    QColor color() const override;

    // ISerializable interface
    void serialize(QVariantMap &data, SerializationContext *pContext) const override;
    void deserialize(const QVariantMap &data, SerializationContext *pContext) override;

protected:

    void processStart() override;
    void processStop() override;
    void process() override;
    void reset() override;

    void noteOnEvent(NoteOnEvent *pEvent) override;
    void noteOffEvent(NoteOffEvent *pEvent) override;

private:

    void createProperties();
    void cachePropetties();

    void doEnvelope();
    void setState(State s);

    void calculateAttack();
    void calculateDecay();
    void calculateRelease();

    OutputPort *m_pOutput;

    int m_noteNumber;
    State m_state;
    float m_output;

    float m_attackTCO;
    float m_decayTCO;
    float m_releaseTCO;

    float m_attackOffset;
    float m_attackCoeff;
    float m_decayOffset;
    float m_decayCoeff;
    float m_releaseOffset;
    float m_releaseCoeff;

    QtVariantProperty *m_pAttackTimeMs;
    QtVariantProperty *m_pDecayTimeMs;
    QtVariantProperty *m_pSustainLevel;
    QtVariantProperty *m_pReleaseTimeMs;
    QtVariantProperty *m_pSignalChainEnable;
    QtVariantProperty *m_pSignalChainDisable;

    float m_dt;
    float m_attackTimeMs;
    float m_decayTimeMs;
    float m_sustainLevel;
    float m_releaseTimeMs;
    bool m_signalChainEnable;
    bool m_signalChainDisable;
};

#endif // AMPLIFIER_H

