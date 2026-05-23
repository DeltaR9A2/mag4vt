
// None of this is really functional code yet.
// I'm sort of designing the game mechanics as code fragments.


typedef struct {
  uint8_t light; // How well lit the location is. Should this be darkness instead?
  uint8_t clutter; // How hard it is to search for things in this location.
} location_t;

typedef struct {
  double mass_in_kilograms; // properties include units
  double volume_in_liters;
  int32_t flags;
} item_t;

enum item_flags {
  VIS_HIDDEN      = (1 <<  0),
  VIS_OBVIOUS     = (1 <<  1),

  SCAN_UV         = (1 <<  2),
  SCAN_RFID       = (1 <<  3),
  SCAN_INFRA      = (1 <<  4),
  SCAN_GEIGER     = (1 <<  5),

}

// Some item properties are calculated like this. Could be cached or not.
uint8_t visibility_of_item( const item_t *i ){
  if( i->volume_in_liters < 0.01 ){
    return 0;
  else if ( i->volume_in_liters < 0.1 ){
    return 1;
  else if ( i->volume_in_liters < 0.5 ){
    return 2;
  else if ( i->volume_in_liters < 1.0 ){
    return 3;
  else if ( i->volume_in_liters < 4.0 ){
    return 4;
  else{
    return 100;
  }
}

typedef struct {

} agent_t;
