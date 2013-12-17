#pragma once

class Animation;


void exportAnimationAsBVH(const Animation *animation);

void renderAnimation(const Animation& animation, const float time = 0.f);
