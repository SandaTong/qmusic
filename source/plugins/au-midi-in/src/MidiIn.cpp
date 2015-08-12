#include <QDebug>
#include <QtVariantPropertyManager>
#include <QtVariantProperty>
#include <qmath.h>
#include "Application.h"
#include "MidiNote.h"
#include "MidiDevice.h"
#include "MidiInputDevice.h"
#include "ISignalChain.h"
#include "../include/MidiIn.h"

const QColor cDefaultColor(230, 240, 210);

MidiIn::MidiIn(AudioUnitPlugin *pPlugin)
    : AudioUnit(pPlugin)
{
    m_pOutputFreq = addOutput("f", Signal::Type_Float);
    m_pOutputVelocity = addOutput("velocity", Signal::Type_Float);
    m_pOutputNoteOn = addOutput("note on", Signal::Type_Bool);

    Application::instance()->midiInputDevice()->addListener(this);

    createProperties();
}

MidiIn::~MidiIn()
{
    Application::instance()->midiInputDevice()->removeListener(this);
}

QColor MidiIn::color() const
{
    return cDefaultColor;
}

void MidiIn::serialize(QVariantMap &data, SerializationContext *pContext) const
{
    Q_ASSERT(pContext != nullptr);
    data["pitchBendSemitones"] = m_pPropPitchBendSemitones->value();
    AudioUnit::serialize(data, pContext);
}

void MidiIn::deserialize(const QVariantMap &data, SerializationContext *pContext)
{
    Q_ASSERT(pContext != nullptr);
    AudioUnit::deserialize(data, pContext);
    m_pPropPitchBendSemitones->setValue(data["pitchBendSemitones"]);
}

void MidiIn::processStart()
{
    m_noteOn = false;
    m_frequency = 0.0f;
    m_frequencyBend = 1.0f;
    m_velocity = 0.0f;
}

void MidiIn::processStop()
{
}

void MidiIn::process()
{
    m_pOutputNoteOn->setBoolValue(m_noteOn);
    m_pOutputFreq->setFloatValue(m_frequency * m_frequencyBend);
    m_pOutputVelocity->setFloatValue(m_velocity);
}

void MidiIn::reset()
{
}

void MidiIn::inputMidiMessage(const MidiMessage &msg)
{
    qDebug() << "[MIDI]" << msg;

    switch (msg.status()) {
    case MidiMessage::Status_NoteOn:
        m_noteOn = true;
        m_frequency = MidiNote(msg.noteNumber()).frequency();
        m_velocity = msg.velocity();
        m_noteNumber = msg.noteNumber();
        break;
    case MidiMessage::Status_NoteOff:
        if (m_noteNumber == msg.noteNumber()) {
            m_noteOn = false;
            //m_frequency = MidiNote(msg.noteNumber()).frequency();
            //m_velocity = double(msg.velocity()) / 127.0;
        }
        break;
    case MidiMessage::Status_PitchBend: {
        int bend = msg.pitchBend() - 8192;
        float dBend = float(bend) / 8192.0f;
        dBend *= m_pPropPitchBendSemitones->value().toDouble();
        m_frequencyBend = pow(2.0, dBend / 12.0);
    }
    default:
        break;
    }
}

void MidiIn::createProperties()
{
    QtVariantProperty *pRoot = rootProperty();

    QtVariantProperty *pPropPitchBend = propertyManager()->addProperty(propertyManager()->groupTypeId(), "Pitch bend");

    m_pPropPitchBendSemitones = propertyManager()->addProperty(QVariant::Int, "Semitones");
    m_pPropPitchBendSemitones->setAttribute("minimum", 1);
    m_pPropPitchBendSemitones->setAttribute("maximum", 24);
    m_pPropPitchBendSemitones->setValue(1);

    pPropPitchBend->addSubProperty(m_pPropPitchBendSemitones);
    pRoot->addSubProperty(pPropPitchBend);
}
