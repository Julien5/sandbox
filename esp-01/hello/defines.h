#pragma once

constexpr int kAntiBoucingMillis = 350;
constexpr int kDefaultSecondsBetweenWifi = 3600L;
// may be quite short, we assume the data are uploaded
// during the day.
constexpr int kRecentlyActiveSeconds = 20;
constexpr int kMinAloneTicks = 2;
constexpr int kSecondsUntilAloneTick = kRecentlyActiveSeconds;
