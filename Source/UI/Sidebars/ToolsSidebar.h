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

#pragma once

//[Headers]
class ProjectTreeItem;

#include "TransportListener.h"
#include "CommandItemComponent.h"

#define TOOLS_SIDEBAR_WIDTH (72)
#define TOOLS_SIDEBAR_ROW_HEIGHT (38)
//[/Headers]

#include "../Themes/PanelBackgroundC.h"
#include "../Themes/PanelBackgroundC.h"
#include "../Themes/SeparatorHorizontalReversed.h"
#include "../Themes/LighterShadowUpwards.h"
#include "../Themes/GradientVertical.h"
#include "../Themes/SeparatorHorizontal.h"
#include "../Common/PlayButton.h"
#include "../Themes/LighterShadowDownwards.h"
#include "../Themes/GradientVerticalReversed.h"

class ToolsSidebar  : public Component,
                      protected TransportListener,
                      protected AsyncUpdater,
                      protected ListBoxModel,
                      protected ChangeListener,
                      protected Timer
{
public:

    ToolsSidebar (ProjectTreeItem &parent);

    ~ToolsSidebar();

    //[UserMethods]
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void handleCommandMessage (int commandId) override;
    void childrenChanged() override;
    void mouseMove (const MouseEvent& e) override;

    // Binary resources:
    static const char* gray1x1_png;
    static const int gray1x1_pngSize;

private:

    //[UserVariables]

    //===------------------------------------------------------------------===//
    // ToolsSidebar
    //===------------------------------------------------------------------===//

    ProjectTreeItem &project;

    double lastSeekTime;
    double lastTotalTime;
    double timerStartSeekTime;
    double timerStartSystemTime;

    ReferenceCountedArray<CommandItem> commandDescriptions;

    void updateModeButtons();
    void emitAnnotationsCallout(Component *newAnnotationsMenu);
    void recreateCommandDescriptions();

    //===------------------------------------------------------------------===//
    // ListBoxModel
    //===------------------------------------------------------------------===//

    int getNumRows() override;
    Component *refreshComponentForRow(int rowNumber, bool isRowSelected,
        Component *existingComponentToUpdate) override;
    void paintListBoxItem(int rowNumber, Graphics &g,
        int width, int height, bool rowIsSelected) override;

    //===------------------------------------------------------------------===//
    // ChangeListener
    //===------------------------------------------------------------------===//

    void changeListenerCallback(ChangeBroadcaster *source) override;

    //===------------------------------------------------------------------===//
    // AsyncUpdater
    //===------------------------------------------------------------------===//

    void handleAsyncUpdate() override;

    //===------------------------------------------------------------------===//
    // Timer
    //===------------------------------------------------------------------===//

    void timerCallback() override;

    //===------------------------------------------------------------------===//
    // TransportListener
    //===------------------------------------------------------------------===//

    void onSeek(const double newPosition, const double currentTimeMs,
        const double totalTimeMs) override;
    void onTempoChanged(const double newTempo) override;
    void onTotalTimeChanged(const double timeMs) override;
    void onPlay() override;
    void onStop() override;

    //[/UserVariables]

    ScopedPointer<PanelBackgroundC> headBg;
    ScopedPointer<PanelBackgroundC> bodyBg;
    ScopedPointer<ListBox> listBox;
    ScopedPointer<SeparatorHorizontalReversed> headLine;
    ScopedPointer<LighterShadowUpwards> shadow;
    ScopedPointer<GradientVertical> gradient2;
    ScopedPointer<SeparatorHorizontal> separator;
    ScopedPointer<Label> totalTime;
    ScopedPointer<Label> currentTime;
    ScopedPointer<PlayButton> playButton;
    ScopedPointer<LighterShadowDownwards> headShadow;
    ScopedPointer<GradientVerticalReversed> gradient;
    ScopedPointer<CommandItemComponent> annotationsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToolsSidebar)
};
