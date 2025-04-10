#include <gui/screen1_screen/Screen1View.hpp>
Screen1View::Screen1View():hour(0),minute(0),seconds(0),tickcount(0)
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}
void Screen1View::handleTickEvent()
{
	tickcount++;
	if(tickcount%60==0)
	{
		if(++seconds>=60)
		{
			seconds=0;
			if(++minute>=60)
			{
				minute=0;
				if(++hour>=24)
				{
					hour=0;
				}
			}
		}
	}
//	seconds++;
//	seconds%=60;
//	if(tickcount==60)
//	{
//		minute++;
//		hour=(hour+(minute/60))%24;
//		minute %=60;
//		tickcount=0;
//	}
	analogClock1.setTime24Hour(hour, minute, seconds);
//	analogClock1.invalidate();

}
