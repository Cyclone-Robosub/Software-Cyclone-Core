from pathlib import Path
from setuptools import find_packages, setup

package_name = 'execute'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (str(Path('share') / package_name / 'launch'), list(map(str, Path('launch').glob('*')))),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='leish',
    maintainer_email='slolei@ucdavis.edu',
    description='TODO: Package description',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
        ],
    },
)
