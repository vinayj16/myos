import React, { useState, useRef, useEffect } from 'react';
import { motion } from 'framer-motion';

const Terminal: React.FC = () => {
  const [history, setHistory] = useState<string[]>([
    'MyOS Terminal v1.1.0',
    'Type "help" for available commands.',
    ''
  ]);
  const [currentInput, setCurrentInput] = useState('');
  const [currentDirectory, setCurrentDirectory] = useState('/home/user');
  const terminalRef = useRef<HTMLDivElement>(null);
  const inputRef = useRef<HTMLInputElement>(null);

  useEffect(() => {
    inputRef.current?.focus();
  }, []);

  useEffect(() => {
    if (terminalRef.current) {
      terminalRef.current.scrollTop = terminalRef.current.scrollHeight;
    }
  }, [history]);

  const commands: { [key: string]: (args: string[]) => string[] } = {
    help: () => [
      'Available commands:',
      '  help, ls, pwd, date, uname, whoami, clear, echo',
      '  cat, mkdir, touch',
      '  git, npm, powershell - (Simulated)',
      ''
    ],
    ls: () => [
      'Desktop/', 'Documents/', 'Downloads/', 'Pictures/',
      'Music/', 'Videos/', 'example.txt', 'script.sh', ''
    ],
    pwd: () => [currentDirectory, ''],
    date: () => [new Date().toString(), ''],
    uname: () => ['MyOS 1.1.0 x86_64', ''],
    whoami: () => ['user', ''],
    clear: () => {
      setHistory(['MyOS Terminal v1.1.0', 'Type "help" for available commands.', '']);
      return [];
    },
    echo: (args) => [args.join(' '), ''],
    cat: (args) => args.length > 0 ? [`Content of ${args[0]}:`, 'This is a sample file.', ''] : ['cat: missing file operand', ''],
    mkdir: (args) => args.length > 0 ? [`Created directory: ${args[0]}`, ''] : ['mkdir: missing operand', ''],
    touch: (args) => args.length > 0 ? [`Created file: ${args[0]}`, ''] : ['touch: missing file operand', ''],
    git: (args) => {
        if (args[0] === 'status') {
            return ["On branch main", "Your branch is up to date with 'origin/main'.", "", "nothing to commit, working tree clean", ""];
        }
        return [`git: '${args[0] || ''}' is not a git command. See 'git --help'.`, ''];
    },
    npm: (args) => {
        if (args[0] === 'install') {
            return ["up to date, audited 1 package in 1s", "found 0 vulnerabilities", ""];
        }
        return [`npm: command not found: ${args[0] || ''}`, ''];
    },
    powershell: () => ["Windows PowerShell", "Copyright (C) Microsoft Corporation. All rights reserved.", "Try the new cross-platform PowerShell https://aka.ms/pscore6", ""]
  };

  const handleCommand = (input: string) => {
    const trimmedInput = input.trim();
    const [command, ...args] = trimmedInput.split(' ');

    setHistory(prev => [...prev, `user@myos:${currentDirectory}$ ${trimmedInput}`]);

    if (command === '') {
      setHistory(prev => [...prev, '']);
      return;
    }

    if (command in commands) {
      const output = commands[command](args);
      if (output.length > 0) {
        setHistory(prev => [...prev, ...output]);
      }
    } else {
      setHistory(prev => [...prev, `Command not found: ${command}`, 'Type "help" for available commands.', '']);
    }
  };

  const handleKeyPress = (e: React.KeyboardEvent) => {
    if (e.key === 'Enter') {
      handleCommand(currentInput);
      setCurrentInput('');
    }
  };

  return (
    <div className="h-full bg-black text-green-400 font-mono text-sm p-4 overflow-hidden flex flex-col" onClick={() => inputRef.current?.focus()}>
      <div 
        ref={terminalRef}
        className="flex-1 overflow-y-auto whitespace-pre-wrap"
      >
        {history.map((line, index) => (
          <motion.div
            key={index}
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            transition={{ duration: 0.1 }}
          >
            {line}
          </motion.div>
        ))}
        <div className="flex">
          <span className="text-yellow-400">user@myos</span>
          <span className="text-white">:</span>
          <span className="text-blue-400">{currentDirectory}</span>
          <span className="text-white">$ </span>
          <input
            ref={inputRef}
            type="text"
            value={currentInput}
            onChange={(e) => setCurrentInput(e.target.value)}
            onKeyPress={handleKeyPress}
            className="flex-1 bg-transparent border-none outline-none text-green-400 caret-green-400"
            autoFocus
          />
        </div>
      </div>
    </div>
  );
};

export default Terminal;
