#include <QDebug>
#include <QtVariantPropertyManager>
#include <QtVariantProperty>
#include <Saxofony.h>
#include "Application.h"
#include "ISignalChain.h"
#include "StkSaxofony.h"

const float cLowestFrequency(20.0);

void setCtrlPropertyAttrs(QtVariantProperty *pProp)
{
    Q_ASSERT(pProp != nullptr);
    pProp->setAttribute("minimum", 0.0);
    pProp->setAttribute("maximum", 1.0);
    pProp->setAttribute("singleStep", 0.1);
    pProp->setValue(0.5);
}

StkSaxofony::StkSaxofony(AudioUnitPlugin *pPlugin)
    : AudioUnit(pPlugin)
{
    m_pInputFreq = addInput("f", Signal::Type_Float);
    m_pInputVelocity = addInput("amp", Signal::Type_Float);
    m_pInputBreath = addInput("breath", Signal::Type_Float);
    m_pInputNoteOn = addInput("on", Signal::Type_Bool);

    m_pOutput = addOutput("out", Signal::Type_Float);

    m_noteOn = false;

    createProperties();

    m_pSaxofony = nullptr;
    try {
        m_pSaxofony = new stk::Saxofony(cLowestFrequency);
    } catch (stk::StkError err) {
        qCritical() << QString::fromStdString(err.getMessage());
    }
}

StkSaxofony::~StkSaxofony()
{
    delete m_pSaxofony;
}

void StkSaxofony::serialize(QVariantMap &data, SerializationContext *pContext) const
{
    Q_ASSERT(pContext != nullptr);
    data["blowPosition"] = m_pPropBlowPosition->value();
    data["reedStiffness"] = m_pPropReedStiffness->value();
    data["reedAperture"] = m_pPropReedAperture->value();
    data["noiseGain"] = m_pPropNoiseGain->value();
    AudioUnit::serialize(data, pContext);
}

void StkSaxofony::deserialize(const QVariantMap &data, SerializationContext *pContext)
{
    Q_ASSERT(pContext != nullptr);
    m_pPropBlowPosition->setValue(data["blowPosition"]);
    m_pPropReedStiffness->setValue(data["reedStiffness"]);
    m_pPropReedAperture->setValue(data["reedAperture"]);
    m_pPropNoiseGain->setValue(data["noiseGain"]);
    AudioUnit::deserialize(data, pContext);
}

void StkSaxofony::processStart()
{
    if (m_pSaxofony != nullptr) {
        m_pSaxofony->setSampleRate(signalChain()->sampleRate());
    }
    m_noteOn = false;
}

void StkSaxofony::processStop()
{
    reset();
}

void StkSaxofony::process()
{
    if (m_pSaxofony == nullptr) {
        return;
    }

    m_pSaxofony->setBlowPosition(m_pPropBlowPosition->value().toDouble());
    m_pSaxofony->controlChange(Ctrl_ReedStiffness, 128.0 * m_pPropReedStiffness->value().toDouble());
    m_pSaxofony->controlChange(Ctrl_ReedAperture, 128.0 * m_pPropReedAperture->value().toDouble());
    m_pSaxofony->controlChange(Ctrl_NoiseGain, 128.0 * m_pPropNoiseGain->value().toDouble());

    bool noteOn = m_pInputNoteOn->value().asBool;
    float freq = m_pInputFreq->value().asFloat;
    freq = qMax(freq, 2.0f * cLowestFrequency);
    float amp = m_pInputVelocity->value().asFloat;
    float breath = m_pInputBreath->value().asFloat;

    if (noteOn && !m_noteOn) {
        // Note goes on
        m_pSaxofony->noteOn(freq, amp);
    } else if (!noteOn && m_noteOn) {
        // Note goes off
        m_pSaxofony->noteOff(amp);
    } else {
        m_pSaxofony->setFrequency(freq);
    }
    m_pSaxofony->controlChange(Ctrl_BreathPressure, 128.0 * breath);

    m_noteOn = noteOn;

    float sample = m_pSaxofony->tick();

    m_pOutput->setFloatValue(sample);
}

void StkSaxofony::reset()
{
    m_noteOn = false;
}

void StkSaxofony::createProperties()
{
    QtVariantProperty *pRoot = rootProperty();

    m_pPropBlowPosition = propertyManager()->addProperty(QVariant::Double, "Blow position");
    setCtrlPropertyAttrs(m_pPropBlowPosition);
    m_pPropReedStiffness = propertyManager()->addProperty(QVariant::Double, "Reed stiffness");
    setCtrlPropertyAttrs(m_pPropReedStiffness);
    m_pPropReedAperture = propertyManager()->addProperty(QVariant::Double, "Reed aperture");
    setCtrlPropertyAttrs(m_pPropReedAperture);
    m_pPropNoiseGain = propertyManager()->addProperty(QVariant::Double, "Noise gain");
    setCtrlPropertyAttrs(m_pPropNoiseGain);

    pRoot->addSubProperty(m_pPropBlowPosition);
    pRoot->addSubProperty(m_pPropReedStiffness);
    pRoot->addSubProperty(m_pPropReedAperture);
    pRoot->addSubProperty(m_pPropNoiseGain);
}
