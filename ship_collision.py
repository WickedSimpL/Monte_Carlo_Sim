import numpy as np
import time
from functools import wraps

def timer_decorator(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        start = time.perf_counter()  # High-precision timer
        result = func(*args, **kwargs)
        end = time.perf_counter()
        print(f"Function '{func.__name__}' executed in: {(end - start):.4f} seconds")
        return result
    return wrapper

# This does assume the ship is a dot rather than a box
def cpa_distance(pos1, vel1, pos2, vel2):
    rel_pos = pos2 - pos1
    rel_vel = vel2 - vel1
    rel_vel_norm2 = np.dot(rel_vel, rel_vel)
    
    if rel_vel_norm2 == 0:
        return np.linalg.norm(rel_pos) # No relative motion: constant distance
    
    t = -np.dot(rel_pos, rel_vel) / rel_vel_norm2
    if t < 0:  
        return np.linalg.norm(rel_pos) # Closest point in the past: use current distance
    
    closest_rel = rel_pos + t * rel_vel
    return np.linalg.norm(closest_rel)

def estimate_crossing_risk(num_trials, num_other_ships, area_size=10.0, max_speed=2.0, threshold_dist=0.2):
    count_risks = 0
    main_pos = np.array([0.0, 0.0])  
    main_vel = np.array([1.0, 0.0])
    
    for _ in range(num_trials):
        has_risk = False
        for _ in range(num_other_ships):
            # Random position for other ship
            other_pos = np.random.uniform(-area_size, area_size, 2)
            # Random direction and speed
            angle = np.random.uniform(0, 2 * np.pi)
            speed = np.random.uniform(0, max_speed)
            other_vel = speed * np.array([np.cos(angle), np.sin(angle)])

            dist = cpa_distance(main_pos, main_vel, other_pos, other_vel)
            if dist < threshold_dist:
                has_risk = True
                break  # At least one crossing detected
        if has_risk:
            count_risks += 1
    return count_risks / num_trials

@timer_decorator
def main():
    num_trials = 50000
    num_other_ships = 15
    area_size = 25.0  # Area is a 20x20 square centered at origin
    max_speed = 2.0  # Other ships up to twice the main ship's speed
    threshold_dist = 0.2  # Safety threshold (e.g., in km or nautical miles)
    prob = (estimate_crossing_risk(num_trials, num_other_ships, area_size, max_speed, threshold_dist) * 100)
    rounded_prob = float(f"{prob:.2f}")
    print(f"\nEstimated probability of collision: {rounded_prob}%")  

if __name__ == "__main__":
    main()