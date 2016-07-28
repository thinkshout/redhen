<?php

namespace Drupal\redhen_note\Entity;

use Drupal\views\EntityViewsData;
use Drupal\views\EntityViewsDataInterface;

/**
 * Provides Views data for RedHen Note entities.
 */
class NoteViewsData extends EntityViewsData implements EntityViewsDataInterface {
  /**
   * {@inheritdoc}
   */
  public function getViewsData() {
    $data = parent::getViewsData();

    $data['note']['table']['base'] = array(
      'field' => 'id',
      'title' => $this->t('RedHen Note'),
      'help' => $this->t('The RedHen Note ID.'),
    );

    return $data;
  }

}
