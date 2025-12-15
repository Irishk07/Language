#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#else // NOT DEBUG
#define ON_DEBUG(...) 
#endif // DEBUG

#ifdef CANARIES
#define ON_CANARY(...) __VA_ARGS__
#else // NOT CANARIES
#define ON_CANARY(...)
#endif // CANARIES

#ifdef TRASH
#define ON_TRASH(...) __VA_ARGS__
#else //NO TRASH
#define ON_TRASH(...)
#endif //TRASH

#endif //DEBUG_H_