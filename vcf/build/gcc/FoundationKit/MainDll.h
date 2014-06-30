#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

struct DLLIMPORT
DllClassBase {
  virtual int virtual_method () const;
};

class DLLIMPORT
DllClass : public DllClassBase {
public:
  DllClass (int i = 0);
  ~DllClass ();
  void non_virtual_method ();
  void virtual_method ();

  static int instances;
private:
  int i_;
};
