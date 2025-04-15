/*
jad file for reference
MIDlet-1: Snake, Snake.png, GameMIDlet
MIDlet-Jar-Size: 7218
MIDlet-Jar-URL: Snake.jar
MIDlet-Name: Snake
MIDlet-Vendor: Unknown
MIDlet-Version: 1.0
MicroEdition-Configuration: CLDC-1.1
MicroEdition-Profile: MIDP-2.1
*/

import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

public class GameMIDlet extends MIDlet{

  SnakeCanvas gameScreen;
  private Display display;

  public GameMIDlet(){
   //constructor
  }
  public void startApp()
  {
      if(gameScreen==null){
          gameScreen = new SnakeCanvas(this);
          gameScreen.start();
          Display.getDisplay(this).setCurrent(gameScreen);
      }else{
          Display.getDisplay(this).setCurrent(gameScreen);
      }
  
    
  }
  
  public Display getDisplay() {
    return display;
  }

  public void pauseApp(){
      gameScreen.stopRunning();
  }
  public void destroyApp(boolean f){
      gameScreen.cleanUP();
      gameScreen.stop();
      gameScreen=null;
      notifyDestroyed();
      System.gc();
  }

}
