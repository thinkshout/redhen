<?php

namespace Drupal\redhen_connection\Plugin\Action;

use Drupal\Core\Action\ConfigurableActionBase;
use Drupal\Core\Entity\DependencyTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\redhen_connection\Entity\ConnectionRole;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides a base class for operations to change a connection's role.
 */
abstract class ChangeConnectionRoleBase extends ConfigurableActionBase implements ContainerFactoryPluginInterface {

  use DependencyTrait;

  /**
   * The connection role entity type.
   *
   * @var \Drupal\Core\Entity\EntityTypeInterface
   */
  protected $entityType;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, EntityTypeInterface $entity_type) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->entityType = $entity_type;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('entity_type.manager')->getDefinition('redhen_connection')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return [
      'role' => '',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    // @todo limit roles by connection type.
    $rids = \Drupal::entityQuery('redhen_connection_role')->execute();
    $role_objects = ConnectionRole::loadMultiple($rids);
    $roles = [];
    foreach ($role_objects as $rid => $role_object) {
      $roles[$rid] = $role_object->label();
    }
    $form['role'] = [
      '#type' => 'radios',
      '#title' => t('Role'),
      '#options' => $roles,
      '#default_value' => $this->configuration['role'],
      '#required' => TRUE,
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitConfigurationForm(array &$form, FormStateInterface $form_state) {
    $this->configuration['role'] = $form_state->getValue('role');
  }

  /**
   * {@inheritdoc}
   */
  public function calculateDependencies() {
    if (!empty($this->configuration['role'])) {
      $prefix = $this->entityType->getConfigPrefix() . '.';
      $this->addDependency('config', $prefix . $this->configuration['role']);
    }
    return $this->dependencies;
  }

  /**
   * {@inheritdoc}
   */
  public function access($object, AccountInterface $account = NULL, $return_as_object = FALSE) {
    /** @var \Drupal\redhen_connection\ConnectionInterface $object */
    $access = $object->access('update', $account, TRUE);
    return $return_as_object ? $access : $access->isAllowed();
  }

}
