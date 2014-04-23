#ifdef DEBUG

#define dbgmsg(message) cout << "[Debug] "  << __FILE__ << ":" << __LINE__ << ": " << message << endl;

#else

#define dbgmsg(message)

#endif
