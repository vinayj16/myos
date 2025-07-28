import React, { useState } from 'react';
import { motion } from 'framer-motion';

const Calculator: React.FC = () => {
  const [display, setDisplay] = useState('0');
  const [expression, setExpression] = useState('');

  const handleButtonClick = (value: string) => {
    if (value === 'C') {
      setDisplay('0');
      setExpression('');
    } else if (value === '=') {
      try {
        // Basic eval, not safe for production but fine for a simulator
        const result = eval(expression.replace('x', '*'));
        setDisplay(result.toString());
        setExpression(result.toString());
      } catch {
        setDisplay('Error');
        setExpression('');
      }
    } else if (value === 'DEL') {
      setDisplay(display.slice(0, -1) || '0');
      setExpression(expression.slice(0, -1));
    } else {
      if (display === '0' || display === 'Error') {
        setDisplay(value);
        setExpression(value);
      } else {
        setDisplay(display + value);
        setExpression(expression + value);
      }
    }
  };

  const buttons = [
    'C', '(', ')', '/',
    '7', '8', '9', 'x',
    '4', '5', '6', '-',
    '1', '2', '3', '+',
    '0', '.', 'DEL', '=',
  ];

  return (
    <div className="h-full flex flex-col bg-gray-100 dark:bg-gray-900">
      <div className="bg-gray-200 dark:bg-gray-800 p-4 rounded-t-lg flex-1 flex items-end justify-end">
        <p className="text-4xl font-light text-gray-900 dark:text-gray-100 break-all">{display}</p>
      </div>
      <div className="grid grid-cols-4 gap-px bg-gray-300 dark:bg-gray-700">
        {buttons.map(btn => (
          <motion.button
            key={btn}
            whileTap={{ scale: 0.95 }}
            onClick={() => handleButtonClick(btn)}
            className={`p-6 text-xl focus:outline-none transition-colors
              ${'C/()x-+=DEL'.includes(btn) ? 'bg-gray-200 dark:bg-gray-800 hover:bg-gray-300 dark:hover:bg-gray-700' : 'bg-white dark:bg-gray-900 hover:bg-gray-100 dark:hover:bg-gray-800'}
              ${btn === '=' ? '!bg-blue-500 hover:!bg-blue-600 text-white' : ''}
              ${btn === 'C' ? 'text-red-500' : 'text-gray-900 dark:text-gray-100'}
            `}
          >
            {btn}
          </motion.button>
        ))}
      </div>
    </div>
  );
};

export default Calculator;
