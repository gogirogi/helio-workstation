/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "TimelineCommandPanel.h"
#include "ProjectTreeItem.h"
#include "MainLayout.h"
#include "Icons.h"
#include "HybridRoll.h"
#include "AnnotationEvent.h"
#include "TimeSignatureEvent.h"
#include "PianoTrackTreeItem.h"
#include "ProjectTimeline.h"
#include "MidiSequence.h"
#include "ModalDialogInput.h"
#include "App.h"

TimelineCommandPanel::TimelineCommandPanel(ProjectTreeItem &parentProject) :
    project(parentProject)
{
    const AnnotationEvent *selectedAnnotation = nullptr;
    const TimeSignatureEvent *selectedTimeSignature = nullptr;

    const ProjectTimeline *timeline = this->project.getTimeline();
    const double seekPosition = this->project.getTransport().getSeekPosition();
    
    if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
    {
        const double numBeats = double(roll->getNumBeats());
        const double seekThreshold = (1.0 / numBeats) / 10.0;
        const auto annotationsSequence = timeline->getAnnotations()->getSequence();
        const auto timeSignaturesSequence = timeline->getTimeSignatures()->getSequence();

        for (int i = 0; i < annotationsSequence->size(); ++i)
        {
            if (AnnotationEvent *annotation = dynamic_cast<AnnotationEvent *>(annotationsSequence->getUnchecked(i)))
            {
                const double annotationSeekPosition = roll->getTransportPositionByBeat(annotation->getBeat());
                if (fabs(annotationSeekPosition - seekPosition) < seekThreshold)
                {
                    selectedAnnotation = annotation;
                    break;
                }
            }
        }

        for (int i = 0; i < timeSignaturesSequence->size(); ++i)
        {
            if (TimeSignatureEvent *ts = dynamic_cast<TimeSignatureEvent *>(timeSignaturesSequence->getUnchecked(i)))
            {
                const double tsSeekPosition = roll->getTransportPositionByBeat(ts->getBeat());
                if (fabs(tsSeekPosition - seekPosition) < seekThreshold)
                {
                    selectedTimeSignature = ts;
                    break;
                }
            }
        }
    }
    
    ReferenceCountedArray<CommandItem> cmds;
    
    if (selectedAnnotation == nullptr)
    {
        cmds.add(CommandItem::withParams(Icons::plus, CommandIDs::AddAnnotation, TRANS("menu::annotation::add")));
    }

    if (selectedTimeSignature == nullptr)
    {
        cmds.add(CommandItem::withParams(Icons::plus, CommandIDs::AddTimeSignature, TRANS("menu::timesignature::add")));
    }
    
    if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
    {
        const auto annotationsSequence = timeline->getAnnotations()->getSequence();

        for (int i = 0; i < annotationsSequence->size(); ++i)
        {
            if (AnnotationEvent *annotation =
                dynamic_cast<AnnotationEvent *>(annotationsSequence->getUnchecked(i)))
            {
                const int commandIndex = (CommandIDs::JumpToAnnotation + i);
                
                double outTimeMs = 0.0;
                double outTempo = 0.0;
                const double seekPos = roll->getTransportPositionByBeat(annotation->getBeat());
                this->project.getTransport().calcTimeAndTempoAt(seekPos, outTimeMs, outTempo);
                
                cmds.add(CommandItem::withParams(Icons::annotation,
                                                 commandIndex,
                                                 annotation->getDescription())->
                         withSubLabel(Transport::getTimeString(outTimeMs))->
                         colouredWith(annotation->getColour()));
            }
        }
    }
    else
    {
        jassertfalse;
    }
    
    this->updateContent(cmds, SlideDown);
}

TimelineCommandPanel::~TimelineCommandPanel()
{
}

void TimelineCommandPanel::handleCommandMessage(int commandId)
{
    if (commandId == CommandIDs::AddAnnotation)
    {
        if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
        {
            roll->postCommandMessage(CommandIDs::AddAnnotation);
            this->getParentComponent()->exitModalState(0);
        }
    }
    else if (commandId == CommandIDs::AddTimeSignature)
    {
        if (HybridRoll *roll = dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
        {
            roll->postCommandMessage(CommandIDs::AddTimeSignature);
            this->getParentComponent()->exitModalState(0);
        }
    }
    else if (commandId == CommandIDs::Cancel)
    {
        if (Component *parent = this->getParentComponent())
        {
            parent->exitModalState(0);
        }
    }
    else
    {
        ProjectTimeline *timeline = this->project.getTimeline();
        const int annotationIndex = (commandId - CommandIDs::JumpToAnnotation);
        
        if (HybridRoll *roll =
            dynamic_cast<HybridRoll *>(this->project.getLastFocusedRoll()))
        {
            if (AnnotationEvent *annotation =
                dynamic_cast<AnnotationEvent *>(timeline->getAnnotations()->getSequence()->getUnchecked(annotationIndex)))
            {
                const double seekPosition = roll->getTransportPositionByBeat(annotation->getBeat());
                this->project.getTransport().seekToPosition(seekPosition);
                roll->scrollToSeekPosition();
            }
        }

        this->getParentComponent()->exitModalState(0);
    }
}
