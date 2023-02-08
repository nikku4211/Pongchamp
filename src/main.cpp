#include "bn_core.h"
#include "bn_log.h"
#include "bn_math.h"
#include "bn_string.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_dmg_music.h"
#include "bn_dmg_music_actions.h"
#include "bn_dmg_music_item.h"
#include "bn_dmg_music_position.h"
#include "bn_random.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_sprite_animate_actions.h"
#include "common_info.h"
#include "common_variable_8x8_sprite_font.h"

#include "bn_sprite_items_ponggers.h"
#include "bn_sprite_items_ballgers.h"
#include "bn_regular_bg_items_circlebg.h"

#include "bn_dmg_music_items_nasutitle.h"
#include "bn_dmg_music_items_nasugameover.h"
#include "bn_dmg_music_items_likeagoodneighbour.h"
#include "bn_dmg_music_items_ping1.h"
#include "bn_dmg_music_items_ping2.h"

int main()
{
    bn::core::init();
    
    bn::regular_bg_ptr circlebg_ptr = bn::regular_bg_items::circlebg.create_bg(8, 48); //display the circle bg
    
    bn::sprite_ptr ballger_ptr = bn::sprite_items::ballgers.create_sprite(0, 0); //create ball sprite
    bn::sprite_ptr pongger_ptr = bn::sprite_items::ponggers.create_sprite(-96, 0); //create paddle sprite
    bn::sprite_ptr pongger2_ptr = bn::sprite_items::ponggers.create_sprite(96, 0); //create paddle sprite
    pongger2_ptr.set_horizontal_flip(true);
    
    int uscore = 0; //you and enemy scores
    int escore = 0;
    
    int uspeed = 2;
    
    bool enemy_rising = false; //is enemy going up?
    
    int deltx = 0; //delta x and y
    int delty = 0;
    
    bn::random random;
    
    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    
    bn::vector<bn::sprite_ptr, 16> text_sprites;
    
    text_generator.generate(-32,48,"Press Start",text_sprites);
    text_generator.generate(-32,64,"Music: Kikiyama,",text_sprites);
    text_generator.generate(-32,72,"Barry Manilow",text_sprites);
    
    bn::dmg_music::play(bn::dmg_music_items::nasutitle, 1, true); //play Nasu title theme
    
    while(true)
    {
        if(bn::keypad::up_held() && pongger_ptr.y() > -48){
          pongger_ptr.set_y(pongger_ptr.y()-uspeed);  //move paddle according to player input
        }else if(bn::keypad::down_held() && pongger_ptr.y() < 48){
          pongger_ptr.set_y(pongger_ptr.y()+uspeed);
        }
        
        if(bn::keypad::b_held()){
          uspeed = 1;  //allow player to change speed
        }else if(bn::keypad::a_held()){
          uspeed = 4;
        }else{
          uspeed = 2;
        }
        
        if(enemy_rising){
          
          if(pongger2_ptr.y() > -48 && ballger_ptr.y() < pongger2_ptr.y()){
            pongger2_ptr.set_y(pongger2_ptr.y()-1);  //move enemy paddle according to ball
          }else{
            enemy_rising = false;
          }
          
        }else{
          if(pongger2_ptr.y() < 48 && ballger_ptr.y() > pongger2_ptr.y()){
            pongger2_ptr.set_y(pongger2_ptr.y()+1);  //move enemy paddle accoding to ball
          }else{
            enemy_rising = true;
          }
        }
        
        if(bn::keypad::start_pressed() && deltx == 0 && delty == 0){
          
          text_sprites.clear();
          bn::string<32> txt_score = "Score: " + bn::to_string<16>(uscore) + " - " + bn::to_string<16>(escore); //score text
          text_generator.generate(-32,64,txt_score,text_sprites);
          
          while(deltx == 0 || delty == 0){
            deltx = (random.get_int() % 5) - 2;
            delty = (random.get_int() % 5) - 2;
          }
          if(!bn::dmg_music::paused()){
          bn::dmg_music::pause(); //stop DMG music
          }
        }
        
        ballger_ptr.set_x(ballger_ptr.x()+deltx);  //move the ball according to delta
        ballger_ptr.set_y(ballger_ptr.y()+delty);
        
        if(ballger_ptr.x() < -100){
          
          if(bn::abs(ballger_ptr.y() - pongger_ptr.y()) < 32){
            deltx *= -1;
            delty = (ballger_ptr.y().integer() - pongger_ptr.y().integer())>>3;
            ballger_ptr.set_horizontal_flip(false);
            bn::dmg_music::play(bn::dmg_music_items::ping1, 1, false); //play DMG ping sound
          }else{
            escore++;
            ballger_ptr.set_position(0,0);
            deltx = (random.get_int() % 5) - 2;
            delty = (random.get_int() % 5) - 2;
            if(deltx == 0){
              deltx = 1;  //zero x speed check
            }
            
            text_sprites.clear();
            bn::string<32> txt_score = "Score: " + bn::to_string<16>(uscore) + " - " + bn::to_string<16>(escore); //score text
            text_generator.generate(-32,64,txt_score,text_sprites);
            if(escore % 10 == 0){
              bn::dmg_music::play(bn::dmg_music_items::nasugameover, 1, false); //play Nasu game over theme
              deltx = 0;
              delty = 0;
            }
          }
          
        }else if(ballger_ptr.x() > 100){
          
          if(bn::abs(ballger_ptr.y() - pongger2_ptr.y()) < 32){
            deltx *= -1;
            delty = (ballger_ptr.y().integer() - pongger2_ptr.y().integer())>>3;
            ballger_ptr.set_horizontal_flip(true);
            bn::dmg_music::play(bn::dmg_music_items::ping2, 1, false); //play DMG ping sound
          }else{
            uscore++;
            ballger_ptr.set_position(0,0);
            deltx = (random.get_int() % 5) - 2;
            delty = (random.get_int() % 5) - 2;
            if(deltx == 0){
              deltx = -1;  //zero x speed check
            }
            
            text_sprites.clear();
            bn::string<32> txt_score = "Score: " + bn::to_string<16>(uscore) + " - " + bn::to_string<16>(escore); //score text
            text_generator.generate(-32,64,txt_score,text_sprites);
            if(uscore % 10 == 0){
              bn::dmg_music::play(bn::dmg_music_items::likeagoodneighbour, 1, false); //play Statefarm theme
              deltx = 0;
              delty = 0;
            }
          }
        }
        
        if(ballger_ptr.y() < -64){
          delty *= -1;  //ball bounces off of ceiling
        }else if(ballger_ptr.y() > 64){
          delty *= -1;  //ball bounces off of floor
        }
        
        if(bn::keypad::r_pressed()){
          BN_LOG(uscore + " - " + escore);
        }
        
        bn::core::update();
    }
}
