"""
Use case: CrystalTexture
========================
"""

import os
import openturns as ot


class CrystalTexture:
    """
    Crystal orientation axes with antipodal symmetry (reduced synthetic stand-ins).

    Bingham-distributed axes on the sphere :math:`\\mathcal{S}^2` mimicking two
    classical axial datasets (see e.g. Mardia et al., 2008):

    - ``ti6al4v``: 300 :math:`c`-axes of rolled Ti-6Al-4V as measured by EBSD
      (Zenodo record 6554355 style: Euler angles reduced to axes), reference
      concentrations ``zeta = [6, 1.5, 0]`` (strong rolling texture);
    - ``protein``: 200 peptide-bond directions as curated from the Protein
      Data Bank (PDB), reference concentrations ``zeta = [4, 1, 0]``
      (weaker preferred orientation).

    Each row is an axis stored as Cartesian components ``(x, y, z)``. Both
    samples were drawn with :class:`~openturns.experimental.Bingham`
    (seeds 34567 and 45678).

    Attributes
    ----------
    ti6al4v : :class:`~openturns.Sample`
        300 :math:`c`-axes (unit vectors).
    protein : :class:`~openturns.Sample`
        200 peptide-bond directions (unit vectors).

    Examples
    --------
    >>> from openturns.usecases import crystal_texture
    >>> data = crystal_texture.CrystalTexture()
    >>> print(data.ti6al4v.getSize(), data.protein.getSize())
    300 200
    """

    def __init__(self):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        self.ti6al4v = ot.Sample.ImportFromCSVFile(
            os.path.join(current_dir, "ti6al4v_c_axes.csv"), ","
        )
        self.protein = ot.Sample.ImportFromCSVFile(
            os.path.join(current_dir, "protein_bond_directions.csv"), ","
        )

    def getReferenceTi6Al4VZeta(self):
        """Reference concentrations of the Ti-6Al-4V texture."""
        return [6.0, 1.5, 0.0]

    def getReferenceProteinZeta(self):
        """Reference concentrations of the protein dataset."""
        return [4.0, 1.0, 0.0]
