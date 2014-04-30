#!/usr/bin/env node  

function convertFen(fen) {
    var board = fen.split(' ')[0];
    var turn = fen.split(' ')[1];

    if (turn == 'w') {turn = 'b'};
    if (turn == 'b') {turn = 'w'};

    var board = board.split('');
    for (var i = 0; i < board.length; i++) {
        if (/[A-Z]/.test(board[i])) {board[i] = board[i].toLowerCase();}
        else if (/[a-z]/.test(board[i])) {board[i] = board[i].toUpperCase();}
    }
    return board.join('') + ' ' + turn;
}
  

var program = require('./commander.js');  
  
//定义参数,以及参数内容的描述  
program  
    .version('0.0.1')  
    .usage('[options] [value ...]')  
    .option('-v, --verbose', 'verbose')
    .option('-f, --fen <string>', 'input FEN string')
    .option('-l, --level <n>', 'AI level (default 3)', parseFloat)
  
//解析commandline arguments  
program.parse(process.argv)  
  
//require('./xqwizard-javascript/position.js');
//require('./xqwizard-javascript/search.js');
//require('./xqwizard-javascript/interface.js');
//require('./xqwizard-javascript/position.js');
var xqwizard = require('./xqwizard.js');

// AI level
if (program.level) {
    var level = program.level;
} else {
    var level = 3;
}

// convert to the right FEN
var fen = program.fen;
if (program.fen.indexOf('k') > program.fen.indexOf('K')) {
    fen = convertFen(program.fen);
}

// make a move
var mv = xqwizard.makeamove(fen, level);

//输出结果  
/*
if (program.verbose) {
    console.log('  input FEN: ', program.fen, '->', fen);
    console.log('    move', mv['move'], ': from', mv['src'], 'to', mv['dst']);
    console.log('  new   FEN: ', mv['fen']);
    console.log('from', mv['from']['x'], mv['from']['y'], 'to', mv['to']['x'], mv['to']['y']);
}

if (mv['end']) {
    return ""
}
*/

var fx = mv['from']['x'];
var fy = mv['from']['y'];
var tx = mv['to']['x'];
var ty = mv['to']['y'];

/*var fx = Math.round((fx*59+30+4) * 1.33);
var fy = Math.round((fy*65.5+31.5+149) * 1.33);
var tx = Math.round((tx*59+30+4) * 1.33);
var ty = Math.round((ty*65.5+31.5+149) * 1.33);
*/
// back convert Fen
if (program.fen.indexOf('k') > program.fen.indexOf('K')) {
    var outputFen = convertFen(mv['fen']).split(' ')[0];
} else {
    var outputFen = mv['fen'].split(' ')[0];
}

if (program.verbose) {
    console.log(fx.toString(), fy.toString(), tx.toString(), ty.toString(), mv['end'], outputFen, 'input:', program.fen, '->', fen, 'from:', mv['from'], 'to:', mv['to'], 'src:', mv['src'], 'dst:', mv['dst'], 'move:', mv['move'],'output:', mv['fen'], 'AI level:', level);
    //console.log(fx, fy, tx, ty, mv['end'], outputFen, 'input:', program.fen, '->', fen, 'from:', mv['from'], 'to:', mv['to'], 'src:', mv['src'], 'dst:', mv['dst'], 'move:', mv['move'],'output:', mv['fen'], 'AI level:', level);
} else {
    console.log(fx.toString(), fy.toString(), tx.toString(), ty.toString(), mv['end'], outputFen);
}

