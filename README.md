# Messy

<img src="docs/images/logo.webp" alt="drawing" width="300"/>

The availability of simulation platforms is a key ingredient for making RISC-V based hardware solutions pervasive. Simulators represent a cost-effective and scalable solution that enable rapid prototyping without the need for physical hardware. For these reasons, there is a growing availability of programs designed to simulate entire RISC-V chips. Some options include QEMU, Simulink, Renode, and GVSoC. However, these tools present some limitations. Most of them only simulate the computing core with scarce possibilities to perform system-level simulations. Moreover, they simulate only functional features, with poor support for extra-functional properties such as power consumption. This work tries to address these challenges by presenting a flexible simulator.

Messy is an open-source framework that integrates functional RISC-V simulation (achieved with GVSoC) with SystemC-AMS (used to model extra-functional aspects, in detail power storage and distribution). The combination of GVSoC and SystemC-AMS in a single simulation framework allows to perform a DSE that is dependent on the mutual impact between functional and extra-functional aspects. 

# Reference

If you enjoy and use Messy on your works, please acknowledge our paper: https://arxiv.org/abs/2404.01861 :
```
@misc{hamdi2024integrating,
      title={Integrating SystemC-AMS Power Modeling with a RISC-V ISS for Virtual Prototyping of Battery-operated Embedded Devices}, 
      author={Mohamed Amine Hamdi and Giovanni Pollo and Matteo Risso and Germain Haugou and Alessio Burrello and Enrico Macii and Massimo Poncino and Sara Vinco and Daniele Jahier Pagliari},
      year={2024},
      eprint={2404.01861},
      archivePrefix={arXiv},
      primaryClass={eess.SY}
}
```

# Documentation

To get started, please refer to the [Documentation](https://eml-eda.github.io/messy/)
