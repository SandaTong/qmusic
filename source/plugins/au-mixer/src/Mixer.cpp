#include <QDebug>
#include <QGraphicsPathItem>
#include <QGraphicsSimpleTextItem>
#include "Application.h"
#include "ISignalChain.h"
#include "../include/Mixer.h"

Mixer::Mixer(AudioUnitPlugin *pPlugin)
    : AudioUnit(pPlugin)
{
    m_pInputA = addInput("", QVariant::Double);
    m_pInputB = addInput("", QVariant::Double);
    m_pOutput = addOutput("", QVariant::Double);
}

Mixer::~Mixer()
{
}

void Mixer::processStart()
{
}

void Mixer::processStop()
{
}

void Mixer::process()
{
    double a = m_pInputA->value().toDouble();
    double b = m_pInputB->value().toDouble();
    m_pOutput->setValue(a + b);
}

QGraphicsItem* Mixer::graphicsItem()
{
    QGraphicsPathItem *pItem = new QGraphicsPathItem();
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(38, 24);
    path.lineTo(0, 48);
    path.closeSubpath();
    pItem->setPath(path);
    pItem->setBrush(QBrush(QColor(0xFF, 0xFF, 0xE0)));

    QGraphicsSimpleTextItem *pPlus = new QGraphicsSimpleTextItem(pItem);
    pPlus->setText("+");
    QFont font = pPlus->font();
    font.setPixelSize(24);
    pPlus->setFont(font);
    pPlus->setPos(4, 7);

    return pItem;
}

int Mixer::flags() const
{
    return Flag_NoTitle | Flag_NoFrame;
}
