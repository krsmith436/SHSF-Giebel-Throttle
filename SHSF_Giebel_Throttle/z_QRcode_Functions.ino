String splitQRcode() {
  String strTemp = "";
  //
  // Arrays to hold the split parts.
  // Size is +1 for null terminator.
  char part1[5] = {0}; // Engine number, 4 characters.
  char part2[5] = {0}; // Road abbreviation, 4 characters.
  char part3[4] = {0}; // Step, 3 characters.
  char part4[5] = {0}; // Minumum forward, 4 characters.
  char part5[5] = {0}; // Maximum forward, 4 characters.
  char part6[6] = {0}; // Minumum reverse, minus sign plus 4 => 5 characters.
  char part7[6] = {0}; // Maximum reverse, minus sign plus 4 => 5 characters.
  char part8[2] = {0}; // Engine facing, 1 character.
  //
  // Copy parts of the original array into separate arrays.
  strncpy(part1, (char*)results.content_bytes, 4); // Copy first 4 characters.
  strncpy(part2, (char*)results.content_bytes + 4, 4); // Copy next 4 characters.
  strncpy(part3, (char*)results.content_bytes + 8, 3); // Copy next 3 characters.
  strncpy(part4, (char*)results.content_bytes + 11, 4); // Copy next 4 characters.
  strncpy(part5, (char*)results.content_bytes + 15, 4); // Copy next 4 characters.
  strncpy(part6, (char*)results.content_bytes + 19, 5); // Copy next 5 characters.
  strncpy(part7, (char*)results.content_bytes + 24, 5); // Copy next 5 characters.
  strncpy(part8, (char*)results.content_bytes + 29, 1); // Copy next 1 character.
  //
  // Drive CAB_A to STOP before setting parameters.
  cabs[CAB_A].throttle = 0;
  driveMotor(CAB_A);
  //
  // Set parameters.
  strcpy(cabs[CAB_A].engineNumber, part1);
  strcpy(cabs[CAB_A].roadAbbreviation, part2);
  // Convert charater arrays to integer.
  cabs[CAB_A].stepValue = atoi(part3); // Step.
  cabs[CAB_A].minForward = atoi(part4); // Minumum forward.
  cabs[CAB_A].maxForward = atoi(part5); // Maximum forward.
  cabs[CAB_A].minReverse = atoi(part6); // Minumum reverse.
  cabs[CAB_A].maxReverse = atoi(part7); // Maximum reverse.
  cabs[CAB_A].engineFacing = atoi(part8); // Engine facing.
  //
  strTemp = String(cabs[CAB_A].roadAbbreviation);
  strTemp.concat(" #");
  strTemp.concat(cabs[CAB_A].engineNumber);
  switch(cabs[CAB_A].engineFacing) {
    case EAST:
      strTemp.concat("   EAST");
      break;
    case WEST:
      strTemp.concat("   WEST");
      break;
    default:
      strTemp.concat("   X");
      break;
  }
  //
  return strTemp;
}