# RandomWeaponList.json
### Fields
```
{
	"Version": 1, // config version (def. 0)
	// Random Weapon list
	"<int>": { // Weapon ID
		"<int>": { // mode flag. 0 = Zombie Hero, 1 = Zombie Hero (Advanced Supply Box), 2 = Zombie Evolution, 3 = Zombie Evolution (Advanced Supply Box), 4 = Zombie Classic, 5 = Zombie Classic (Advanced Supply Box) (def. 0)
			"DropRate": <int>, // rate of appearance. official server has these values: 111, 222, 333, 444, 555. the higher the number, the more likely the weapon will be chosen (def. 0)
			"EnhanceProbability": <int> // probability of the weapon being maximum enhanced, ranges from 0 to 100 (def. 0)
		}
	}
}
```
def. means that if the field is undefined, the default value will be used.