String splitQRcode() {
  String strTemp = "";
  //
  // Arrays to hold the split parts.
  // Size is +1 for null terminator.
  char part1[5] = {0}; // Engine number, 4 characters.
  char part2[4] = {0}; // Step, 3 characters.
  char part3[5] = {0}; // Minumum forward, 4 characters.
  char part4[5] = {0}; // Maximum forward, 4 characters.
  char part5[6] = {0}; // Minumum reverse, minus sign plus 4 => 5 characters.
  char part6[6] = {0}; // Maximum reverse, minus sign plus 4 => 5 characters.
  char part7[2] = {0}; // Engine facing, 1 character.
  //
  // Copy parts of the original array into separate arrays.
  strncpy(part1, (char*)results.content_bytes, 4); // Copy first 4 characters.
  strncpy(part2, (char*)results.content_bytes + 4, 3); // Copy next 3 characters.
  strncpy(part3, (char*)results.content_bytes + 7, 4); // Copy next 4 characters.
  strncpy(part4, (char*)results.content_bytes + 11, 4); // Copy next 4 characters.
  strncpy(part5, (char*)results.content_bytes + 15, 5); // Copy next 5 characters.
  strncpy(part6, (char*)results.content_bytes + 20, 5); // Copy next 5 characters.
  strncpy(part7, (char*)results.content_bytes + 25, 1); // Copy next 1 character.
  //
  // Convert charater arrays to integer.
  strcpy(cabs[CAB_A].engineNumber, part1);
  cabs[CAB_A].stepValue = atoi(part2); // Step.
  cabs[CAB_A].minForward = atoi(part3); // Minumum forward.
  cabs[CAB_A].maxForward = atoi(part4); // Maximum forward.
  cabs[CAB_A].minReverse = atoi(part5); // Minumum reverse.
  cabs[CAB_A].maxReverse = atoi(part6); // Maximum reverse.
  cabs[CAB_A].engineFacing = atoi(part7); // Engine facing.

  strTemp = F("Eng: ");
  strTemp.concat(cabs[CAB_A].engineNumber);
  

  // Print the results
  Serial.println("\nSplit Arrays:");
  Serial.print("Part 1: ");
  Serial.println(part1);

  Serial.print("Part 2: ");
  Serial.println(cabs[CAB_A].stepValue);

  Serial.print("Part 3: ");
  Serial.println(cabs[CAB_A].minForward);

  Serial.print("Part 4: ");
  Serial.println(cabs[CAB_A].maxForward);

  Serial.print("Part 5: ");
  Serial.println(cabs[CAB_A].minReverse);

  Serial.print("Part 6: ");
  Serial.println(cabs[CAB_A].maxReverse);

  Serial.print("Part 7: ");
  Serial.println(cabs[CAB_A].engineFacing);
  //
  return strTemp;
}