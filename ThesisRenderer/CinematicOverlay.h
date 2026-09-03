#pragma once

struct GLFWwindow;

class CinematicOverlay
{
public:
    bool enabled =
        false;

    float blackBarHeight =
        90.0f;

    void HandleShortcut(
        GLFWwindow* window
    );

    void Draw(
        int screenWidth,
        int screenHeight
    ) const;

private:
    bool togglePressed =
        false;
};