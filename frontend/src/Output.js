//This is the table component, currently all the values are hardcoded

import * as React from 'react';
import Table from '@mui/material/Table';
import TableBody from '@mui/material/TableBody';
import TableCell from '@mui/material/TableCell';
import TableContainer from '@mui/material/TableContainer';
import TableHead from '@mui/material/TableHead';
import TableRow from '@mui/material/TableRow';
import Paper from '@mui/material/Paper';

function createData(name, value, unit) {
  return { name, value, unit};
}

const rows = [
  createData("Sensor 1", "100", "N"),
  createData("Sensor 2", "50", "Feet"),
  createData("Sensor 3", "25", "Meters"),
  createData("Sensor 4", "12", "PSI"),
  
];

export default function Output() {
  return (
    <TableContainer component={Paper}>
      <Table sx={{ minWidth: 350, minHeight: 600}} aria-label="simple table">
        <TableHead>
          <TableRow>
            <TableCell>Sensors</TableCell>
            <TableCell align="right">Value</TableCell>
            <TableCell align="right">Unit</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {rows.map((row) => (
            <TableRow
              key={row.name}
              sx={{ '&:last-child td, &:last-child th': { border: 0 } }}
            >
              <TableCell component="th" scope="row">
                {row.name}
              </TableCell>
              <TableCell align="right">{row.value}</TableCell>
              <TableCell align="right">{row.unit}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </TableContainer>
  );
}