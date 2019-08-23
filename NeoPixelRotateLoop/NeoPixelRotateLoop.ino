// NeoPixelFunLoop
// This example will move a trail of light around a series of pixels.  
// A ring formation of pixels looks best.  
// The trail will have a slowly fading tail.
// 
// This will demonstrate the use of the RotateRight method.
//

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>


const uint16_t PixelCount = 111; // make sure to set this to the number of pixels in your strip
const uint16_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
const uint16_t AnimCount = 1; // we only need one
const uint16_t TailLength = 55; // length of the tail, must be shorter than PixelCount
const float MaxLightness = 0.5f; // max lightness at the head of the tail (0.5f is full bright)
const float MinLightness = 0.15f;
const float hue = 0.3f;

NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
// for esp8266 omit the pinan
//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount);

struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
    uint16_t IndexPixel; // which pixel this animation is effecting
};

NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object


void LoopAnimUpdate(const AnimationParam& param)
{
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed)
    {

        
        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);

        // rotate the complete strip one pixel to the right on every update
        strip.RotateRight(1);

        
    }
}

/*
void BlendAnimUpdate(const AnimationParam& param)
{

    if (param.state == AnimationState_Completed)
    {
        for (uint16_t index = 0; index < strip.PixelCount(); index++)
        {
          HslColor oldColor = strip.GetPixelColor(index);
          float newHue = oldColor.H + 0.001f;
          if(newHue > 1.0f) newHue = 0.0f;
          RgbColor newColor = HslColor(newHue, oldColor.S, oldColor.L);
          strip.SetPixelColor(index, colorGamma.Correct(newColor));
        }      

        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);
    }
        
}*/

void DrawTailPixels()
{
    
    float hue = 0.3f;
    for (uint16_t index = 0; index <= strip.PixelCount() / 2/*&& index <= TailLength*/; index++)
    {
        float lightness = index * MaxLightness / TailLength;
        if (lightness < MinLightness) lightness = MinLightness;
        if (lightness > MaxLightness) lightness = MaxLightness;
        RgbColor color = HslColor(hue, 1.0f, lightness);

        strip.SetPixelColor(index, colorGamma.Correct(color));
        strip.SetPixelColor(strip.PixelCount()-index, colorGamma.Correct(color));
    }

    for (uint16_t index = 0; index <= strip.PixelCount(); index++)
    { 
        HslColor color = strip.GetPixelColor(index);
        /*if(color.L < MinLightness)
        {
          color.L = MinLightness;
          RgbColor newColor = RgbColor(color);
          strip.SetPixelColor(index, colorGamma.Correct(newColor));
        }*/
        
        /*Serial.print(index);
        Serial.print(":");
        Serial.print(color.L);
        Serial.println();*/
    }
}

void setup()
{
    //Serial.begin(115200);
    strip.Begin();
    strip.Show();

    //SetRandomSeed();

    // Draw the tail that will be rotated through all the rest of the pixels
    DrawTailPixels();

    
    animations.StartAnimation(0, 25, LoopAnimUpdate); 
    //animations.StartAnimation(1, 30, BlendAnimUpdate);
}


void loop()
{
    // this is all that is needed to keep it running
    // and avoiding using delay() is always a good thing for
    // any timing related routines
    animations.UpdateAnimations();
    strip.Show();
}
