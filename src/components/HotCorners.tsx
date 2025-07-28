import React from 'react';

interface HotCornersProps {
  onShowWorkspaces: () => void;
  onShowDesktop: () => void;
}

const HotCorners: React.FC<HotCornersProps> = ({ onShowWorkspaces, onShowDesktop }) => {
  return (
    <>
      {/* Top-left: Show Workspaces */}
      <div
        className="fixed top-0 left-0 w-4 h-4 z-50"
        onMouseEnter={onShowWorkspaces}
      />
      {/* Top-right: Placeholder */}
      <div
        className="fixed top-0 right-0 w-4 h-4 z-50"
      />
      {/* Bottom-left: Placeholder */}
      <div
        className="fixed bottom-0 left-0 w-4 h-4 z-50"
      />
      {/* Bottom-right: Show Desktop */}
      <div
        className="fixed bottom-0 right-0 w-4 h-4 z-50"
        onMouseEnter={onShowDesktop}
      />
    </>
  );
};

export default HotCorners;
