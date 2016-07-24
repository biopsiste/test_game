#pragma once

unsigned int move_timer_cb(unsigned int interval, void *param) {
  SDL_Event event;
  SDL_UserEvent userevent;

  /* In this example, our callback pushes an SDL_USEREVENT event
  into the queue, and causes our callback to be called again at the
  same interval: */

  userevent.type = SDL_USEREVENT;
  userevent.code = 0;
  userevent.data1 = NULL;
  userevent.data2 = NULL;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return(interval);
}



