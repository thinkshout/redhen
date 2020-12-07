<?php

namespace Drupal\redhen_connection\Plugin\views\access;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\views\Plugin\views\access\AccessPluginBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\Routing\Route;

/**
 * Access plugin for RedHen Connections.
 *
 * @ingroup views_access_plugins
 *
 * @ViewsAccess(
 *   id = "redhen_connection_access",
 *   title = @Translation("RedHen connection access"),
 *   help = @Translation("Restrict access based on the current user's RedHen connections.")
 * )
 */
class ConnectionAccess extends AccessPluginBase {

  /**
   * {@inheritdoc}
   */
  protected $usesOptions = TRUE;

  /**
   * The currently active route match object.
   *
   * @var \Drupal\Core\Routing\RouteMatchInterface
   */
  protected $routeMatch;

  /**
   * The RedHen connection access checker.
   *
   * @var \Drupal\redhen_connection\Access\ConnectionAccessCheck
   */
  protected $connectionAccessCheck;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    $instance = parent::create($container, $configuration, $plugin_id, $plugin_definition);
    $instance->routeMatch = $container->get('current_route_match');
    $instance->connectionAccessCheck = $container->get('redhen_connection.access_check');
    return $instance;
  }

  /**
   * List of available operation options.
   *
   * @return array
   *   List of operations.
   */
  public function getOperations() {
    return [
      'view' => $this->t('View'),
      'view label' => $this->t('View label'),
      'update' => $this->t('Update'),
      'delete' => $this->t('Delete'),
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function access(AccountInterface $account) {
    return $this->connectionAccessCheck->access($this->routeMatch, $account);
  }

  /**
   * {@inheritdoc}
   */
  public function alterRouteDefinition(Route $route) {
    $operation = $this->options['operation'] ?? 'view';
    $route->setRequirement('_redhen_connection_access', $operation);
  }

  /**
   * {@inheritdoc}
   */
  public function summaryTitle() {
    $operation = $this->options['operation'] ?? 'view';
    $operations = $this->getOperations();
    return $operations[$operation];
  }

  protected function defineOptions() {
    $options = parent::defineOptions();
    $options['operation'] = ['default' => 'view'];
    return $options;
  }

  public function buildOptionsForm(&$form, FormStateInterface $form_state) {
    parent::buildOptionsForm($form, $form_state);

    $operation = $this->options['operation'] ?? 'view';
    $operations = $this->getOperations();

    $form['operation'] = [
      '#type' => 'select',
      '#options' => $operations,
      '#title' => $this->t('Operation'),
      '#default_value' => $operation,
      '#description' => $this->t('Only users with the ability to perform this operation on the RedHen entity in the url path will be able to access this display.<br>Note: this only applies to routes that contain {redhen_contact} or {redhen_org} parameters.'),
    ];
  }

}
