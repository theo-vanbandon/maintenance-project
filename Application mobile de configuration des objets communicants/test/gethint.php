<?php
$mysqli = new mysqli("172.17.129.128", "UserVibration", "vibrations", "BDD_Vibration");
if($mysqli->connect_error) {
  exit('Could not connect');
}

$sql = "SELECT GUID_machine FROM machine WHERE id = ?";

$stmt = $mysqli->prepare($sql);
$stmt->bind_param("s", $_GET['q']);
$stmt->execute();
$stmt->store_result();
$stmt->bind_result($guid);
$stmt->fetch();
$stmt->close();

echo "<table>";
echo "<tr>";
echo "<th>CustomerID</th>";
echo "<td>" . $guid . "</td>";
echo "</tr>";
echo "</table>";
?>