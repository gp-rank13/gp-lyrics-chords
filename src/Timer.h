// Lyrics and Chords extension for Gig Performer by @rank13

#pragma once

#include <juce_events/juce_events.h>

class ClockTimer : public Timer
{
public:
    void timerCallback() override;
};

class RefreshTimer : public Timer
{
public:
    void timerCallback() override;
};

class CaratTimer : public Timer
{
public:
    void timerCallback() override;
};

class CreateImageTimer : public Timer
{
public:
    void timerCallback() override;
};

class WindowChangeTimer : public Timer
{
public:
    void timerCallback() override;
};

class SongScrollTimer : public juce::HighResolutionTimer
{
    void hiResTimerCallback() override;
};

class SongScrollPauseTimer : public juce::HighResolutionTimer
{
    void hiResTimerCallback() override;
};